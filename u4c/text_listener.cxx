#include "u4c/text_listener.hxx"
#include "u4c/testnode.hxx"
#include "except.h"

namespace u4c {
using namespace std;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void
text_listener_t::begin()
{
    nrun_ = 0;
    nfailed_ = 0;
    fprintf(stderr, "u4c: running\n");
}

void
text_listener_t::end()
{
    fprintf(stderr, "u4c: %u run %u failed\n",
	    nrun_, nfailed_);
}

void
text_listener_t::begin_node(const testnode_t *tn)
{
    fprintf(stderr, "u4c: running: \"%s\"\n", tn->get_fullname().c_str());
    result_ = u4c::R_UNKNOWN;
}

void
text_listener_t::end_node(const testnode_t *tn)
{
    string fullname = tn->get_fullname();

    nrun_++;
    switch (result_)
    {
    case R_PASS:
	fprintf(stderr, "PASS %s\n", fullname.c_str());
	break;
    case R_NOTAPPLICABLE:
	fprintf(stderr, "N/A %s\n", fullname.c_str());
	break;
    case R_FAIL:
	nfailed_++;
	fprintf(stderr, "FAIL %s\n", fullname.c_str());
	break;
    default:
	fprintf(stderr, "??? (result %d) %s\n", result_, fullname.c_str());
	break;
    }
}

void
text_listener_t::add_event(const u4c_event_t *ev, functype_t ft)
{
    const char *type;
    char buf[2048];

    switch (ev->which)
    {
    case EV_ASSERT: type = "ASSERT"; break;
    case EV_EXIT: type = "EXIT"; break;
    case EV_SIGNAL: type = "SIGNAL"; break;
    case EV_SYSLOG: type = "SYSLOG"; break;
    case EV_FIXTURE: type = "FIXTURE"; break;
    case EV_EXPASS: type = "EXPASS"; break;
    case EV_EXFAIL: type = "EXFAIL"; break;
    case EV_EXNA: type = "EXNA"; break;
    case EV_VALGRIND: type = "VALGRIND"; break;
    case EV_SLMATCH: type = "SLMATCH"; break;
    default: type = "unknown"; break;
    }
    snprintf(buf, sizeof(buf), "EVENT %s %s",
		type, ev->description);
    if (*ev->filename && ev->lineno)
	snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf),
		 " at %s:%u",
		 ev->filename, ev->lineno);
    if (*ev->function)
	snprintf(buf+strlen(buf), sizeof(buf)-strlen(buf),
		 " in %s %s",
		 as_string(ft),
		 ev->function);
    strcat(buf, "\n");
    fputs(buf, stderr);
}

void
text_listener_t::finished(result_t res)
{
    result_ = res;
}

// close the namespace
};