#include <stdio.h>
#include <map>
#include <string>
#include <asm-generic/unistd.h>
#include "pin.H"

struct trace_t
{
  std::string fname;
  ADDRINT faddr;
  ADDRINT caller;
};

KNOB<bool> TraceApis(KNOB_MODE_WRITEONCE, "pintool", "f", "1", "Trace API calls");
KNOB<bool> TraceSyscalls(KNOB_MODE_WRITEONCE, "pintool", "s", "0", "Profile Syscalls");

trace_t node;
unsigned long syscallcounter = 0;

std::map<ADDRINT, unsigned long> syscalls;
std::map<ADDRINT, std::string> api_names;
std::vector<trace_t> tracer;


void usage()
{
  std::string h = KNOB_BASE::StringKnobSummary();

  fprintf(stderr, "\npicotracer - API/Syscall call tracer v0.1\n");
  fprintf(stderr, "%s\n", h.c_str());
}


void output_data(INT32 code, void *v)
{
  unsigned long count;
  std::vector<trace_t>::iterator it;
  std::map<ADDRINT, unsigned long>::iterator jt;

  if(!tracer.empty())
  {
    printf("\nAddress             IP                  API\n");
    printf("----------------------------------------------------------------------\n");

    for(it = tracer.begin(); it != tracer.end(); ++it)
      printf("0x%016jx  0x%016jx  %s\n", it->faddr, it->caller, it->fname.c_str());
  }

  if(!syscalls.empty())
  {
    printf("\nSyscall      Stats\n");
    printf("----------------------------------------------------------------------\n");

    for(jt = syscalls.begin(); jt != syscalls.end(); jt++)
    {
      count = jt->second;
      printf(" %-8ju  %3lu (%0.2f%%)\n", jt->first, count, (double)count/syscallcounter*100.0);
    }
  }
}


void api_analysis_f(ADDRINT ip, ADDRINT target)
{
  node.faddr = target;
  node.fname = api_names[target].c_str(); 
  node.caller = ip;
  tracer.push_back(node);
}


void syscall_analysis_f(THREADID t, CONTEXT *ctxt, SYSCALL_STANDARD std, VOID *v)
{
  syscalls[PIN_GetSyscallNumber(ctxt, std)]++;
  syscallcounter++;
}


void apis_instrumentation_f(INS ins, void *v)
{
  if(!INS_IsBranchOrCall(ins))
    return;

  IMG img = IMG_FindByAddress(INS_Address(ins));

  if(!IMG_Valid(img) || !IMG_IsMainExecutable(img))
    return;

  if(INS_IsCall(ins))
  {
    if(TraceApis.Value())
    {
      INS_InsertCall(
        ins, IPOINT_BEFORE, (AFUNPTR)api_analysis_f, 
        IARG_INST_PTR, IARG_BRANCH_TARGET_ADDR,  
        IARG_END
      );
    }
  }
}


void save_function_names_g(IMG img, void *v)
{
  if(!IMG_Valid(img))
    return;

  for(SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
    for(RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
      api_names[RTN_Address(rtn)] = RTN_Name(rtn);
}


int main(int argc, char *argv[])
{
  PIN_InitSymbols();
  if(PIN_Init(argc,argv))
  {
    usage();
    return 0;
  }

  IMG_AddInstrumentFunction(save_function_names_g, NULL);
  INS_AddInstrumentFunction(apis_instrumentation_f, NULL);

  if(TraceSyscalls.Value())
    PIN_AddSyscallEntryFunction(syscall_analysis_f, NULL);

  PIN_AddFiniFunction(output_data, NULL);

  PIN_StartProgram();
    
  return 0;
}

