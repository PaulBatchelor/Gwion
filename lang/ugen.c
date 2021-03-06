#include "vm.h"
#include "type.h"
#include "dl.h"
#include "err_msg.h"
#include "import.h"
#include "lang.h"
#include "ugen.h"
#include "bbq.h"

/*#include <soundpipe.h>*/

struct Type_ t_ugen = { "UGen", sizeof(m_uint), &t_object, te_ugen };

m_bool base_tick(UGen u)
{
  m_uint i, size = vector_size(u->ugen);
  UGen ugen;
  if(!size)
  {
    u->out = 0;
    return 1;
  }
  ugen = vector_at(u->ugen, 0);
  u->out = ugen->out;
/*  u->out = ugen ? ugen->out : 0;*/
  for(i = 1; i < size; i++)
  {
    ugen = vector_at(u->ugen, i);
/*    if(!ugen)*/
/*      continue;*/
    switch(u->op)
    {
      case 1:
        u->out += ugen->out;
        break;
      case 2:
        u->out -= ugen->out;
        break;
      case 3:
        u->out *= ugen->out;
        break;
      case 4:
        u->out /= ugen->out;
        break;
    }
  }
  u->in = u->out;
  return 1;
}

m_bool blackhole_tick(UGen u)
{
  base_tick(u);
}

m_bool dac_tick(UGen u)
{
  m_uint  i, j;
  sp_data* sp = (sp_data*)u->ug;
  for(i = 0; i < sp->nchan; i++)
    sp->out[i] = 0; 
  for(i = 0; i < u->n_out; i++)
    sp->out[i] += u->channel[i]->ugen->out;
  return 1;
}

m_bool adc_tick(UGen u)
{
  m_uint  i, j;
  m_float last = 0;
  BBQ sp = (BBQ )u->ug;
  for(i = 0; i < u->n_out; i++)
  {
    M_Object obj = u->channel[i];
    last += (obj->ugen->out = sp->in[i]);
  }
  last /= u->n_out;
  return 1;
}
void ref_compute(UGen u)
{
  u->tick(u);
  u->done = 1;
}

void channel_compute(UGen u)
{
//	if(u->done)
//		return;
	m_uint i;
	u->in = 0;
	for(i = 0; i < vector_size(u->ugen); i++)
		u->in += ((UGen)vector_at(u->ugen, i))->out;
//	u->done = 1;
}
// recursively compute ugen
void ugen_compute(UGen u)
{
  m_uint  i, j;
  m_float sum = 0;
  UGen ugen;
  if(!u || u->done)
    return;
  if(u->channel)
    for(i = 0; i < u->n_out; i++)
      ugen_compute(u->channel[i]->ugen);
  else
    for(i = 0; i < vector_size(u->ugen); i++)
    {
      ugen = (UGen)vector_at(u->ugen, i);
      if(!ugen) // find a way to remove ugens
/*      continue;*/
      exit(12);
      if(!ugen->done)
        ugen_compute(ugen);
    }
  if(u->ref)
  {
    u->tick(u);
    for(i = 0; i < u->n_in; i++)
      u->ref->channel[i]->ugen->tick(u->ref->channel[i]->ugen);
    ref_compute(u->ref);
    return; 
  }
  u->tick(u); 
  if(u->channel)
  {
/*    for(i = 0; i < u->n_out> u->n_in ? u->n_out : u->n_in; i++)*/
    for(i = 0; i < u->n_out; i++)
    {
      M_Object obj = u->channel[i];
      sum += obj->ugen->out;
    }
    u->last = sum /u->n_out;
  }
  else
    u->last = u->out;
  u->done = 1;
}

UGen new_UGen()
{
  UGen u    =  (UGen) calloc(1, sizeof(struct UGen_));
  u->ugen   = NULL;
  u->channel = NULL;
  u->ug = NULL;
  u->trig = NULL;
/*  u->to = NULL;*/
  u->tick = base_tick;
  u->done = 0;
  u->op = 1;
  return u;
}

void free_UGen(UGen u)
{
  if(u->ugen)
    free_Vector(u->ugen);
  else
    free(u->channel);
  free(u);
}

M_Object new_M_UGen()
{
  M_Object o = new_M_Object();
	initialize_object(o, &t_ugen);
  o->ugen = new_UGen();
	return o;
}

m_bool assign_ugen(UGen u, m_uint n_in, m_uint n_out, m_bool trig, void* ug)
{
  u->n_chan = n_in > n_out ? n_in : n_out;
  if(u->n_chan > 1)
  {
    u->channel = calloc(u->n_chan, sizeof(M_Object));
    m_uint i;
    for(i = 0; i < u->n_chan; i++)
    {
      M_Object chan = new_M_UGen();
      assign_ugen(chan->ugen, n_in > i, n_out > i, 0, NULL);
      chan->ugen->tick = base_tick;
      chan->ugen->ref = u;
      u->channel[i] =  chan;
    }
  }
  else
    u->ugen = new_Vector();
  if(trig)
  {
    u->trig = new_M_UGen();
    assign_ugen(u->trig->ugen, 1, 1, 0, NULL);
    u->trig->ugen->tick = base_tick;
  }
  u->in = u->out = 0;
  u->n_in   = n_in;
  u->n_out  = n_out;
  u->ug     = ug;
	return 1;
}

static void ugen_connect(VM* vm, VM_Shred shred, Instr instr)
{
#ifdef DEBUG_INSTR
  debug_msg("instr", "ugen connect %p %p", *(m_uint*)(shred->reg - SZ_INT*2), *(m_uint*)(shred->reg - SZ_INT));
#endif
  m_uint i;
  shred->reg -= SZ_INT*2;
	M_Object lhs = *(M_Object*)shred->reg;
  M_Object rhs = *(M_Object*)(shred->reg + SZ_INT);

  if(!rhs->ugen->n_in)
  {
    shred->is_done =1;
    shred->is_running = 0;
    err_msg(INSTR_, 0, "'%s' has no inputs", rhs->type_ref->name);
    return;
  }
  if(rhs->ugen->channel)
  {
    for(i = 0; i < rhs->ugen->n_out; i++)
    {
      M_Object obj = rhs->ugen->channel[i];
			if(lhs->ugen->n_out > 1)
				vector_append(obj->ugen->ugen, lhs->ugen->channel[i%lhs->ugen->n_out]->ugen);
      else
				vector_append(obj->ugen->ugen,    lhs->ugen);
    }
  }
  else
    vector_append(rhs->ugen->ugen, lhs->ugen);
  release(lhs, shred);
  release(rhs, shred);
	*(M_Object*)shred->reg = rhs;
	shred->reg += SZ_INT;
}

static void ugen_disconnect(VM* vm, VM_Shred shred, Instr instr)
{
#ifdef DEBUG_INSTR
  debug_msg("instr", "ugen connect %p %p", **(m_uint**)(shred->reg - SZ_INT*2), **(m_uint**)(shred->reg - SZ_INT));
#endif
  m_uint i;
  shred->reg -= SZ_INT*2;
  M_Object lhs = *(M_Object*)shred->reg;
  M_Object rhs = *(M_Object*)(shred->reg + SZ_INT);
  if(!rhs->ugen->n_in)
  {
    shred->is_done =1;
    shred->is_running = 0;
    return;
  }
  if(rhs->ugen->channel)
  {
    for(i = 0; i < rhs->ugen->n_out; i++)
    {
      M_Object obj = rhs->ugen->channel[i];
      UGen ugen = obj->ugen;
      vector_remove(ugen->ugen, vector_find(ugen->ugen,  lhs->ugen));
    }
  }
  else
    vector_remove(rhs->ugen->ugen, vector_find(rhs->ugen->ugen, (m_uint*)lhs->ugen));
  release(lhs, shred);
  release(rhs, shred);
	*(M_Object*)shred->reg = rhs;
	shred->reg += SZ_INT;
}

static void trig_connect(VM* vm, VM_Shred shred, Instr instr)
{
#ifdef DEBUG_INSTR
  debug_msg("instr", "trig connect %p %p", **(m_uint**)(shred->reg - SZ_INT*2), **(m_uint**)(shred->reg - SZ_INT));
#endif
  m_uint i;
  shred->reg -= SZ_INT*2;
  M_Object lhs = *(M_Object*)shred->reg;
  M_Object rhs = *(M_Object*)(shred->reg + SZ_INT);
  if(rhs->ugen->trig)
    vector_append(rhs->ugen->trig->ugen->ugen, lhs->ugen);
  release(lhs, shred);
  release(rhs, shred);
	*(M_Object*)shred->reg = rhs;
	shred->reg += SZ_INT;
}

static void trig_disconnect(VM* vm, VM_Shred shred, Instr instr)
{
#ifdef DEBUG_INSTR
  debug_msg("instr", "trig discconnect %p %p", **(m_uint**)(shred->reg - SZ_INT*2), **(m_uint**)(shred->reg - SZ_INT));
#endif
  m_uint i;
  shred->reg -= SZ_INT*2;
  M_Object lhs = *(M_Object*)shred->reg;
  M_Object rhs = *(M_Object*)(shred->reg + SZ_INT);
  if(rhs->ugen->trig)
    vector_remove(rhs->ugen->trig->ugen->ugen, vector_find(rhs->ugen->trig->ugen->ugen,  lhs->ugen));
  release(lhs, shred);
  release(rhs, shred);
	*(M_Object*)shred->reg = rhs;
	shred->reg += SZ_INT;
}

void ugen_ctor(M_Object o, VM_Shred shred)
{
  o->ugen = new_UGen();
/*  o->ugen->to = new_Vector();*/
  vector_append(shred->vm_ref->ugen, (m_uint*)o->ugen);
}


void ugen_dtor(M_Object o, VM_Shred shred)
{
  m_uint i;
  UGen u;
  // remove for vm ugen vector
  vector_remove(shred->vm_ref->ugen, (m_uint)vector_find(shred->vm_ref->ugen, o->ugen));
/*  if(o->ugen->to)*/
/*    for(i = 0; i < vector_size(o->ugen->to); i++)*/
/*    {*/
/*      u = (UGen)vector_at(o->ugen->to, i);*/
/*      if(!u)*/
/*        continue;*/
/*    vector_remove(u->ugen, (m_uint)vector_find(u->ugen, o->ugen));*/
/*    } */
    free_UGen(o->ugen);
    o->ugen = NULL;
}

static void ugen_channel(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_int i = *(m_int*)(shred->mem + SZ_INT);
  if(!o->ugen->channel && !i)
    RETURN->v_object = o;
	else if(i < 0 || i >= o->ugen->n_out)
    RETURN->v_object = NULL;
  else RETURN->v_object = o->ugen->channel[i];
}

static void ugen_get_op(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  RETURN->v_uint = o->ugen->op;
}

static void ugen_set_op(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_int i = *(m_int*)(shred->mem + SZ_INT);
  if(i < 1 || i > 4)
    err_msg(INSTR_, 0, "invalid op %i", i);
  else 
    o->ugen->op = i;
  RETURN->v_uint = o->ugen->op;
}

static void ugen_get_last(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  if(!o)
    RETURN->v_float = 0;
  if(!o->ugen)
    exit(2);
  else RETURN->v_float = o->ugen->last;
}

m_bool import_ugen(Env env)
{
  DL_Func* fun;

  CHECK_BB(add_global_type(env, &t_ugen))
  CHECK_BB(import_class_begin(env, &t_ugen, env->global_nspc, ugen_ctor, ugen_dtor))
	env->class_def->doc = "base sound class";

  fun = new_DL_Func("UGen", "chan", (m_uint)ugen_channel);
    dl_func_add_arg(fun, "int", "arg0");
  CHECK_OB(import_mfun(env, fun))
  
  fun = new_DL_Func("int", "op", (m_uint)ugen_get_op);
  CHECK_OB(import_mfun(env, fun))
  
  fun = new_DL_Func("int", "op", (m_uint)ugen_set_op);
    dl_func_add_arg(fun, "int", "arg0");
  CHECK_OB(import_mfun(env, fun))
  
  fun = new_DL_Func("float", "last", (m_uint)ugen_get_last);
  CHECK_OB(import_mfun(env, fun))

  CHECK_BB(add_binary_op(env, op_chuck,   &t_ugen, &t_ugen, &t_ugen, ugen_connect, 1))
  CHECK_BB(add_binary_op(env, op_unchuck, &t_ugen, &t_ugen, &t_ugen, ugen_disconnect, 1))
  CHECK_BB(add_binary_op(env, op_trig,    &t_ugen, &t_ugen, &t_ugen, trig_connect, 1))
  CHECK_BB(add_binary_op(env, op_untrig,  &t_ugen, &t_ugen, &t_ugen, trig_disconnect, 1))
  
  CHECK_BB(import_class_end(env))
  return 1;
}
