#include "spiegel/spiegel.hxx"
#include "spiegel/dwarf/state.hxx"
using namespace std;

static int
test_info(int argc, char **argv)
{
    bool preorder = true;
    const char *filename = 0;
    for (int i = 1 ; i < argc ; i++)
    {
	if (!strcmp(argv[i], "--preorder"))
	{
	    preorder = true;
	}
	else if (!strcmp(argv[i], "--recursive"))
	{
	    preorder = false;
	}
	else if (argv[i][0] == '-')
	{
usage:
	    fatal("Usage: spiegtest info [--preorder|--recursive] EXE\n");
	}
	else
	{
	    if (filename)
		goto usage;
	    filename = argv[i];
	}
    }

    spiegel::dwarf::state_t state(filename);
    state.map_sections();
    state.read_compile_units();
    state.dump_info(preorder);

    return 0;
}

static int
test_abbrevs(int argc, char **argv)
{
    if (argc != 2)
	fatal("Usage: spiegtest abbrevs EXE\n");

    spiegel::dwarf::state_t state(argv[1]);
    state.map_sections();
    state.read_compile_units();
    state.dump_abbrevs();

    return 0;
}

static int
test_functions_dwarf(int argc, char **argv)
{
    if (argc != 2)
	fatal("Usage: spiegtest functions EXE\n");

    spiegel::dwarf::state_t state(argv[1]);
    state.map_sections();
    state.read_compile_units();
    state.dump_functions();

    return 0;
}

static int
test_variables(int argc, char **argv)
{
    if (argc != 2)
	fatal("Usage: spiegtest variables EXE\n");

    spiegel::dwarf::state_t state(argv[1]);
    state.map_sections();
    state.read_compile_units();
    state.dump_variables();

    return 0;
}

static int
test_structs(int argc, char **argv)
{
    if (argc != 2)
	fatal("Usage: spiegtest structs EXE\n");

    spiegel::dwarf::state_t state(argv[1]);
    state.map_sections();
    state.read_compile_units();
    state.dump_structs();

    return 0;
}

static int
test_read_uleb128(int argc __attribute__((unused)),
		  char **argv __attribute__((unused)))
{
#define TESTCASE(in, out) \
    { \
	printf(". read_uleb128(%u) ", out); fflush(stdout); \
	spiegel::dwarf::reader_t r(in, sizeof(in)-1); \
	uint32_t v; \
	if (!r.read_uleb128(v) || v != out) \
	{ \
	    printf("FAIL\n"); \
	    return 1; \
	} \
	printf("PASS\n"); \
    }

    TESTCASE("\x02", 2);
    TESTCASE("\x7f", 127);
    TESTCASE("\x80\x01", 128);
    TESTCASE("\x81\x01", 129);
    TESTCASE("\x82\x01", 130);
    TESTCASE("\xb9\x64", 12857);
#undef TESTCASE
    return 0;
}

static int
test_read_sleb128(int argc __attribute__((unused)),
		  char **argv __attribute__((unused)))
{
#define TESTCASE(in, out) \
    { \
	printf(". read_sleb128(%d) ", out); fflush(stdout); \
	spiegel::dwarf::reader_t r(in, sizeof(in)-1); \
	int32_t v; \
	if (!r.read_sleb128(v) || v != out) \
	{ \
	    printf("FAIL\n"); \
	    return 1; \
	} \
	printf("PASS\n"); \
    }

    TESTCASE("\x02", 2);
    TESTCASE("\x7e", -2);
    TESTCASE("\xff\x00", 127);
    TESTCASE("\x81\x7f", -127);
    TESTCASE("\x80\x01", 128);
    TESTCASE("\x80\x7f", -128);
    TESTCASE("\x81\x01", 129);
    TESTCASE("\xff\x7e", -129);

#undef TESTCASE
    return 0;
}

static int
test_compile_units(int argc, char **argv)
{
    if (argc != 2)
	fatal("Usage: spiegtest compile_units EXE\n");

    spiegel::dwarf::state_t state(argv[1]);
    state.map_sections();
    state.read_compile_units();

    printf("Compile Units\n");
    printf("=============\n");

    vector<spiegel::compile_unit_t *> units = spiegel::compile_unit_t::get_compile_units();
    vector<spiegel::compile_unit_t *>::iterator i;
    for (i = units.begin() ; i != units.end() ; ++i)
    {
	printf("%s/%s\n", (*i)->get_compile_dir(), (*i)->get_name());
    }

    printf("\n\n");

    return 0;
}

static int
test_functions(int argc, char **argv)
{
    if (argc != 2)
	fatal("Usage: spiegtest functions EXE\n");

    spiegel::dwarf::state_t state(argv[1]);
    state.map_sections();
    state.read_compile_units();

    printf("Functions\n");
    printf("=========\n");

    vector<spiegel::compile_unit_t *> units = spiegel::compile_unit_t::get_compile_units();
    vector<spiegel::compile_unit_t *>::iterator i;
    for (i = units.begin() ; i != units.end() ; ++i)
    {
	printf("%s/%s\n", (*i)->get_compile_dir(), (*i)->get_name());

	vector<spiegel::function_t *> fns = (*i)->get_functions();
	vector<spiegel::function_t *>::iterator j;
	for (j = fns.begin() ; j != fns.end() ; ++j)
	{
	    int n = 0;

	    printf("    %s %s(",
		    (*j)->get_return_type()->to_string().c_str(),
		    (*j)->get_name());

	    vector<spiegel::type_t *> atypes = (*j)->get_parameter_types();
	    vector<spiegel::type_t *>::iterator k;
	    for (k = atypes.begin() ; k != atypes.end() ; ++k)
	    {
		if (n++)
		    printf(", ");
		printf("%s", (*k)->to_string().c_str());
	    }

	    if ((*j)->has_unspecified_parameters())
	    {
		if (n)
		    printf(", ");
		printf("...");
	    }

	    printf(")\n");
	}
    }

    printf("\n\n");

    return 0;
}



int
main(int argc, char **argv)
{
    argv0 = argv[0];
    if (!strcmp(argv[1], "info"))
	return test_info(argc-1, argv+1);
    if (!strcmp(argv[1], "abbrevs"))
	return test_abbrevs(argc-1, argv+1);
    if (!strcmp(argv[1], "functions"))
	return test_functions(argc-1, argv+1);
    if (!strcmp(argv[1], "functions_dwarf"))
	return test_functions_dwarf(argc-1, argv+1);
    if (!strcmp(argv[1], "variables"))
	return test_variables(argc-1, argv+1);
    if (!strcmp(argv[1], "structs"))
	return test_structs(argc-1, argv+1);
    if (!strcmp(argv[1], "read_uleb128"))
	return test_read_uleb128(argc-1, argv+1);
    if (!strcmp(argv[1], "read_sleb128"))
	return test_read_sleb128(argc-1, argv+1);
    if (!strcmp(argv[1], "compile_units"))
	return test_compile_units(argc-1, argv+1);
    fatal("Usage: spiegel command args...\n");
    return 1;
}

