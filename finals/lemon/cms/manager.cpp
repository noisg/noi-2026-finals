#include "testlib.h"
#include <csignal>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>

using namespace std;


/******************************** Begin testlib-related material ********************************/

inline FILE* openFile(const char* name, const char* mode) {
	FILE* file = fopen(name, mode);
	if (!file)
		quitf(_fail, "Could not open file '%s' with mode '%s'.", name, mode);
	closeOnHalt(file);
	return file;
}


vector<FILE*> mgr2sol, sol2mgr;
FILE* log_file = nullptr;

void nullifyFile(int idx) {
	mgr2sol[idx] = sol2mgr[idx] = nullptr;
}

#ifdef __GNUC__
__attribute__ ((format (printf, 1, 2)))
#endif
void log_printf(const char* fmt, ...) {
	if (log_file) {
		FMT_TO_RESULT(fmt, fmt, message);
		fprintf(log_file, "%s", message.c_str());
		fflush(log_file);
	}
}

void registerManager(std::string probName, int num_processes, int argc, char* argv[]) {
	setName("manager for problem %s", probName.c_str());
	__testlib_ensuresPreconditions();
	testlibMode = _checker;
	random_t::version = 1;
	__testlib_set_binary(stdin);
	ouf.mode = _output;

	{
		struct sigaction sa;
		sa.sa_handler = SIG_IGN;
		sigaction(SIGPIPE, &sa, NULL);
	}

	int required_args = 1 + 2 * num_processes;
	if (argc < required_args || required_args+1 < argc) {
		string usage = format("'%s'", argv[0]);
		for (int i = 0; i < num_processes; i++)
			usage += format(" sol%d-to-mgr mgr-to-sol%d", i, i);
		usage += " [mgr_log] < input-file";
		quitf(_fail,
			"Manager for problem %s:\n"
			"Invalid number of arguments: %d\n"
			"Usage: %s",
			probName.c_str(), argc-1, usage.c_str());
	}

	inf.init(stdin, _input);
	closeOnHalt(stdout);
	closeOnHalt(stderr);

	mgr2sol.resize(num_processes);
	sol2mgr.resize(num_processes);
	for (int i = 0; i < num_processes; i++) {
		mgr2sol[i] = openFile(argv[1 + 2*i + 1], "a");
		sol2mgr[i] = openFile(argv[1 + 2*i + 0], "r");
	}

	if (argc > required_args) {
		log_file = openFile(argv[required_args], "w");
	} else {
		log_file = nullptr;
	}
}
/********************************* End testlib-related material *********************************/

// grader/manager protocol

const int secret_g2m = 0x852CA190;
const int secret_m2g = 0xD248E6C0;
const int code_mask  = 0x0000000F;

const int M2G_CODE__OK = 0;
const int M2G_CODE__DIE = 1;

const int G2M_CODE__OK = 0;
const int G2M_CODE__PV_CALL_EXIT = 13;
const int G2M_CODE__PV_TAMPER_M2G = 14;
const int G2M_CODE__SILENT = 15;

const int PYTHON_MARKER = 0x50595448; // 'PYTH'


int fifo_idx = 0;

void out_flush() {
	fflush(mgr2sol[fifo_idx]);
}

void write_int(int x) {
	FILE* fout = mgr2sol[fifo_idx];
	if (1 != fwrite(&x, sizeof(x), 1, fout)) {
		nullifyFile(fifo_idx);
		log_printf("Could not write int to mgr2sol[%d]\n", fifo_idx);
	}
}

void write_int_array(const int* arr, int len) {
	FILE* fout = mgr2sol[fifo_idx];
	if (int ret = fwrite(arr, sizeof(int), len, fout); len != ret) {
		nullifyFile(fifo_idx);
		log_printf("Could not write int array of size %d to mgr2sol[%d], fwrite returned %d\n", len, fifo_idx, ret);
	}
}

void write_int_vector(const vector<int>& v) {
	write_int_array(v.data(), v.size());
}

void write_secret(int m2g_code = M2G_CODE__OK) {
	write_int(secret_m2g | m2g_code);
}

#ifdef __GNUC__
__attribute__ ((format (printf, 2, 3)))
#endif
NORETURN void die(TResult result, const char* format, ...) {
	FMT_TO_RESULT(format, format, message);
	log_printf("Dying with message '%s'\n", message.c_str());
	for (int i = 0; i < (int)mgr2sol.size(); ++i)
		if(mgr2sol[i] != nullptr) {
			fifo_idx = i;
			log_printf("Sending secret with code DIE to mgr2sol[%d]\n", fifo_idx);
			write_secret(M2G_CODE__DIE);
			out_flush();
		}
	log_printf("Quitting with result code %d\n", int(result));
	quit(result, message);
}

NORETURN void die_rte(const string &msg) {
	RESULT_MESSAGE_WRONG = "Runtime Error";
	die(_wa, "%s", msg.c_str());
}

int read_int() {
	FILE* fin = sol2mgr[fifo_idx];
	int x;
	if (1 != fread(&x, sizeof(x), 1, fin)) {
		nullifyFile(fifo_idx);
		die_rte("manual RTE, cant read int from grader");
	}
	return x;
}

void read_secret() {
	int secret = read_int();
	if (secret == PYTHON_MARKER)
		die(_wa, "This problem only accepts C++ submissions");
	if((secret & ~code_mask) != secret_g2m)
		die(_pv, "Possible tampering with sol2mgr[%d]", fifo_idx);
	int g2m_code = secret & code_mask;
	switch (g2m_code) {
		case G2M_CODE__OK:
			return;
		case G2M_CODE__SILENT:
			die(_fail, "Unexpected g2m_code SILENT from sol2mgr[%d]", fifo_idx);
		case G2M_CODE__PV_TAMPER_M2G:
			die(_pv, "Possible tampering with mgr2sol[%d]", fifo_idx);
		case G2M_CODE__PV_CALL_EXIT:
			die(_pv, "Solution[%d] called exit()", fifo_idx);
		default:
			die(_fail, "Unknown g2m_code %d from sol2mgr[%d]", g2m_code, fifo_idx);
	}
}


// ========================== Lemon problem logic ==========================

int main(int argc, char **argv) {
	registerManager("lemon", 2, argc, argv);

	int subtask = inf.readInt();
	int T = inf.readInt();

	bool all_correct = true;
	int max_X = 0, max_Y = 0;

	// Per-test data stored during Alice phase, used during Bob phase
	struct TestData {
		int N, L;
		vector<int> b;       // bitstring (as ints, 0/1)
		vector<int> uneaten; // sorted uneaten fruit ids
	};
	vector<TestData> tests(T);

	// --- Phase 1: All Alice phases, in order ---
	fifo_idx = 0;
	for (int tc = 0; tc < T; tc++) {
		int N = inf.readInt();
		int L = inf.readInt();
		vector<int> p(N);
		for (int i = 0; i < N; i++)
			p[i] = inf.readInt();

		write_secret();
		write_int(subtask);
		write_int(N);
		write_int_vector(p);
		write_int(L);
		out_flush();

		read_secret();
		int X = read_int();
		if (X < 0)
			die(_wa, "TC %d: init() returned a bitstring with negative length %d", tc, X);
		if (X > 5000)
			die(_wa, "TC %d: init() returned a bitstring of length %d, which exceeds the limit of 5000", tc, X);

		vector<int> b(X);
		if (X > 0) {
			for (int i = 0; i < X; i++) {
				b[i] = read_int();
				if (b[i] != 0 && b[i] != 1)
					die(_wa, "TC %d: init() returned a bitstring containing invalid character (value %d at position %d); only '0' and '1' are allowed", tc, b[i], i);
			}
		}

		if (X > max_X) max_X = X;

		// Read uneaten list from stub (stub runs per-fruit loop locally)
		int K = read_int();
		if (K < 0 || K > N)
			die(_wa, "TC %d: stub returned invalid uneaten count %d", tc, K);
		vector<int> uneaten(K);
		for (int i = 0; i < K; i++)
			uneaten[i] = read_int();

		if (std::find(uneaten.begin(), uneaten.end(), L) == uneaten.end())
			die(_wa, "TC %d: contestant ate the lemon", tc);

		int Y = N - K;
		if (Y > max_Y) max_Y = Y;

		sort(uneaten.begin(), uneaten.end());

		tests[tc] = {N, L, std::move(b), std::move(uneaten)};
	}

	// Send end sentinel to process 0 (Alice)
	write_secret();
	write_int(-1);
	out_flush();
	nullifyFile(0);

	// Shuffle test order for Bob using testlib RNG
	vector<int> bob_order(T);
	iota(bob_order.begin(), bob_order.end(), 0);
	shuffle(bob_order.begin(), bob_order.end());

	// --- Phase 2: All Bob phases, in shuffled order ---
	fifo_idx = 1;
	for (int idx : bob_order) {
		const auto& td = tests[idx];
		int X = (int)td.b.size();
		int K = (int)td.uneaten.size();

		write_secret();
		write_int(subtask);
		write_int(td.N);
		write_int(X);
		if (X > 0)
			write_int_vector(td.b);
		write_int(K);
		if (K > 0)
			write_int_vector(td.uneaten);
		out_flush();

		read_secret();
		int ans = read_int();

		if (ans < 1 || ans > td.N)
			die(_wa, "TC %d: answer() returned %d, which is not a valid fruit id (expected 1..%d)", idx, ans, td.N);

		if (ans != td.L) {
			int Y = td.N - K;
			log_printf("TC %d: WRONG - Bob answered %d, expected %d (X=%d, Y=%d)\n",
					   idx, ans, td.L, X, Y);
			all_correct = false;
		} else {
			int Y = td.N - K;
			log_printf("TC %d: OK (X=%d, Y=%d)\n", idx, X, Y);
		}
	}

	// Send end sentinel to process 1 (Bob)
	write_secret();
	write_int(-1);
	out_flush();
	nullifyFile(1);

	log_printf("Max X = %d, Max Y = %d\n", max_X, max_Y);

	if (!all_correct) {
		quitf(_wa, "Wrong answer. Max X = %d, Max Y = %d", max_X, max_Y);
	}

	// Compute partial score
	double frac;
    if (subtask == 1) {
        if (max_Y > 2) frac = 0.0;
        else if (max_X == 0) frac = 1.0;
        else frac = std::min(288.0 / max_X, 1.0);
    } else if (subtask == 2) {
        if (max_Y > 9) frac = 0.0;
        else if (max_X == 0) frac = 1.0;
        else frac = std::min(30.0 / max_X, 1.0);
    } else {
        int sum = max_X + max_Y;
        if (sum == 0) frac = 1.0;
        else frac = std::min(20.0 / sum, 1.0);
    }

	if (frac == 1.0) {
		quitf(_ok, "Subtask %d: X = %d, Y = %d", subtask, max_X, max_Y);
	} else {
		fprintf(stderr, "X = %d, Y = %d\n", max_X, max_Y);
		quitp(frac, "Subtask %d: X = %d, Y = %d", subtask, max_X, max_Y);
	}

	return 0;
}
