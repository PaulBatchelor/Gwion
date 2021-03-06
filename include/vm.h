#ifndef __VM
#define __VM

#include <complex.h>
#include "defs.h"
#include "map.h"

//#include "driver.h" // put Driver in def ?

typedef struct VM_Code_* VM_Code;
typedef enum { NATIVE_UNKNOWN, NATIVE_CTOR, NATIVE_DTOR, NATIVE_MFUN, NATIVE_SFUN } e_native_func;
struct VM_Code_
{
  Vector instr;
  m_str name, filename;
  m_uint stack_depth;
  m_uint native_func;
  e_native_func native_func_type;
  m_bool need_this;
};

typedef struct BBQ_* BBQ;
typedef struct Shreduler_* Shreduler;
typedef struct
{
  Vector shred, ugen;
	BBQ bbq;
	Shreduler shreduler;
  M_Object adc, dac, blackhole;
  Emitter emit;
  Env env;
//	Driver driver;
} VM;

typedef struct VM_Shred_* VM_Shred;
struct VM_Shred_
{
  VM_Code code;
  VM_Shred parent;
//  Stack reg;
//  Stack mem;
  char* reg;
  char* mem;
  char* base;
  m_uint pc, next_pc, xid;
	m_str name;
	m_bool is_running, is_done;
	VM* vm_ref;
	VM_Shred prev, next;
	m_float wake_time;
  Vector args;
  M_Object me;
	m_str filename;
	Vector child;
//	m_int* i_ret;
//	m_float* f_ret;
//	complex* c_ret;
};

VM_Code new_VM_Code(Vector instr, m_uint stack_depth, m_bool need_this, m_str name, m_str filename);
void free_VM_Code(VM_Code a);
Shreduler new_Shreduler(VM* vm);
void vm_add_shred(VM* vm, VM_Shred shred);
m_bool shreduler_remove(Shreduler s, VM_Shred out, m_bool erase);

VM_Shred new_VM_Shred(VM_Code code);
void free_VM_Shred(VM_Shred shred);
void set_now(Shreduler s, m_uint* pos);
void shreduler(Shreduler s, m_uint* pos);
VM_Shred shreduler_get(Shreduler s);
void vm_run(VM* vm);
VM* new_VM();
void free_VM(VM* vm);
#endif
