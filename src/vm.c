#include <stdlib.h>
#include "vm.h"

// get rid of that
#include "err_msg.h"
#include "type.h"

#include "instr.h"
#include "ugen.h"

extern m_uint ssp_is_running;
VM_Code new_VM_Code(Vector instr, m_uint stack_depth, m_bool need_this, m_str name, m_str filename)
{
  VM_Code code = calloc(1, sizeof(struct VM_Code_));
  code->instr = instr;
  code->stack_depth = stack_depth;
  code->need_this = need_this;
  code->name= name;
  code->filename = filename;
	code->native_func_type = NATIVE_UNKNOWN;
  return code;
}

void free_VM_Code(VM_Code a)
{
  free(a);
}

VM_Shred new_VM_Shred(VM_Code c)
{
  VM_Shred shred    = calloc(1, sizeof(struct VM_Shred_));
  shred->mem        = calloc(0x1 << 16, sizeof(char));
  shred->reg        = calloc(0x1 << 14, sizeof(char));
  shred->base       = shred->mem;
  shred->pc         = 0;
  shred->next_pc    = 0;
  shred->code       = c;
  shred->is_done    = 0;
  shred->is_running = 1;
  shred->wake_time  = 0.;
  shred->xid        = -1;
	shred->prev       = NULL;
	shred->next       = NULL;
  shred->name       = c->name;
  shred->me         = NULL;
  shred->child      = NULL;
  shred->parent     = NULL;
  return shred;
}

void free_VM_Shred(VM_Shred shred)
{
  free(shred->mem);
  free(shred->reg);
  free(shred);
}

VM* new_VM()
{
  VM* vm     = (VM*)calloc(1, sizeof(VM));
  vm->shred  = new_Vector();
  vm->ugen   = new_Vector();
	vm->shreduler = new_Shreduler(vm);
  return vm;
}

void free_VM(VM* vm)
{
  free_Vector(vm->shred);
  free_Vector(vm->ugen);
  sp_destroy(&vm->bbq->sp);
  free(vm->bbq);
  free_Shreduler(vm->shreduler);
  free(vm);
}

void vm_add_shred(VM* vm, VM_Shred shred)
{
  shred->vm_ref = vm;
  if(shred->xid == -1)
  {
    shred->xid = vector_size(vm->shred);
    vector_append(vm->shred, shred);
  }
	shredule(vm->shreduler, shred, get_now(vm->shreduler) + .5);
}

void vm_run(VM* vm)
{
  m_uint   i;
  VM_Shred shred;
  Instr    instr;
  UGen     ugen;
  while((shred = shreduler_get(vm->shreduler)))
  {
#ifdef DEBUG_VM
/*  else */
  debug_msg("vm", "shred [%i]: stack: {%i:%i}. pc: (%i,%i / %i)", shred->xid, *shred->reg,
      *shred->mem, shred->pc, shred->next_pc, vector_size(shred->code->instr));


/*  debug_msg("vm", "shred [%i]: pc: (%i,%i / %i)", shred->xid,*/
/*      shred->pc, shred->next_pc, vector_size(shred->code->instr));*/

#endif
    while(shred->is_running)
    {
      shred->pc = shred->next_pc;
      shred->next_pc++;
      instr = vector_at(shred->code->instr, shred->pc);
#ifdef DEBUG_VM
      if(!instr)
      {
        err_msg(VM_, 0, "internal error: no instruction");
  debug_msg("vm", "shred [%i]: pc: (%i,%i / %i)", shred->xid,
      shred->pc, shred->next_pc, vector_size(shred->code->instr));
	VM_Shred sh =*(VM_Shred*)shred;
  debug_msg("vm", "shred [%i]: pc: (%i,%i / %i)", sh->xid,
      sh->pc, sh->next_pc, vector_size(sh->code->instr));

break;
//continue;
//        shred->is_done = 1;
      }
      else if(!instr->execute)
      {
        err_msg(VM_, 0, "internal error: instruction has no execute function");
continue;
        shred->is_done = 1;
      }
      else
#endif
      instr->execute(vm, shred, instr);
      if(shred->is_done)
        shreduler_remove(vm->shreduler, shred, 1);
#ifdef DEBUG_VM
/*  else */
  debug_msg("vm", "shred [%i]: stack: {%i:%i}. pc: (%i,%i / %i)", shred->xid, *shred->reg,
      *shred->mem, shred->pc, shred->next_pc, vector_size(shred->code->instr));
#endif
    }
  }
  if(!ssp_is_running)
    return;

/*printf("lol %i\n", vector_size(vm->ugen));*/
  if(vector_size(vm->ugen) > 3)
  for(i = 0; i < vector_size(vm->ugen); i++)
  {
    UGen u = vector_at(vm->ugen, i);
/*printf("u %p u->channel %p\n", u, u->channel);*/
    u->done = 0;
    if(u->channel)
    {
/*      if(u->n_in)*/
      for(int j = 0; j < u->n_in; j++)
/*printf("u->channel[j] %p\n", u->channel);*/
        u->channel[j]->ugen->done = 0;
      for(int j = 0; j < u->n_out; j++)
      {
/*        printf("%p->channel[%i]->done %i\n", u, j, u->channel[j]->ugen->done);*/
        u->channel[j]->ugen->done = 0;
      }
    }
    if(u->ref)
      exit(122);
/*      u->ref->done = 0;*/
  }
  ugen_compute(vm->blackhole->ugen);
  ugen_compute(vm->dac->ugen);
  ugen_compute(vm->adc->ugen);
}

