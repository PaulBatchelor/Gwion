#include <math.h>
#include "vm.h"
#include "type.h"
#include "dl.h"
#include "err_msg.h"
#include "import.h"
#include "ugen.h"
#include "func.h"
#include "bbq.h"

extern struct Type_ t_osc;
static struct Type_ t_sinosc      = { "SinOsc",      1, &t_osc };
typedef struct
{
  sp_data* sp;
  sp_osc* osc;
  char is_init;
  sp_ftbl*  tbl;
  m_float   phase;
} SP_osc; // copied from generated osc.c

static void sinosc_ctor(M_Object o, VM_Shred shred)
{
  SP_osc* ug = (SP_osc*)o->ugen->ug;
  sp_ftbl_create(shred->vm_ref->bbq->sp, &ug->tbl, 2048);
  sp_gen_sine(shred->vm_ref->bbq->sp, ug->tbl);
  sp_osc_init(shred->vm_ref->bbq->sp, (sp_osc*)ug->osc, ug->tbl, 0.);
  ug->is_init = 1;
}

static void sinosc_size(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  int size = *(m_int*)(shred->mem + SZ_INT);
  if(size <= 0)
  {
    err_msg(INSTR_, 0, "%s size requested for sinosc. doing nothing",
      size < 0 ? "negative" : "zero");
    return;
  }
  SP_osc* ug = (SP_osc*)o->ugen->ug;
  sp_ftbl_destroy(&ug->tbl);
  sp_osc_destroy(&ug->osc);
  sp_osc_create(&ug->osc);
  sp_ftbl_create(shred->vm_ref->bbq->sp, &ug->tbl, size);
  sp_gen_sine(shred->vm_ref->bbq->sp, ug->tbl);
  sp_osc_init(shred->vm_ref->bbq->sp, (sp_osc*)ug->osc, ug->tbl, 0.);
}

static void sinosc_size_phase(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  int size    = *(m_int*)(shred->mem + SZ_INT);
  float phase = *(m_int*)(shred->mem + SZ_INT*2);
  if(size <= 0)
  {
    err_msg(INSTR_, 0, "%s size requested for sinosc. doing nothing",
      size < 0 ? "negative" : "zero");
    return;
  }
  SP_osc* ug = (SP_osc*)o->ugen->ug;
  sp_ftbl_destroy(&ug->tbl);
  sp_osc_destroy(&ug->osc);
  sp_osc_create(&ug->osc);
  sp_ftbl_create(shred->vm_ref->bbq->sp, &ug->tbl, size);
  sp_gen_sine(shred->vm_ref->bbq->sp, ug->tbl);
  sp_osc_init(shred->vm_ref->bbq->sp, (sp_osc*)ug->osc, ug->tbl, phase);
}

static m_bool import_sinosc(Env env){
  Func     f;
  DL_Func* fun;
	CHECK_BB(add_global_type(env, &t_sinosc))
	CHECK_BB(import_class_begin(env, &t_sinosc, env->global_nspc, sinosc_ctor, NULL))
	env->class_def->doc = "a simple sinusoid derived from 'Osc'.";
  fun = new_DL_Func("void", "init", (m_uint)sinosc_size);
   dl_func_add_arg(fun, "int", "size");
  CHECK_BB((f = import_mfun(env, fun)))
  f->doc = "alloc other sizes for sinosc. (default: 2048)";
  fun = new_DL_Func("void", "init", (m_uint)sinosc_size_phase);
   dl_func_add_arg(fun, "int", "size");
   dl_func_add_arg(fun, "float", "phase");
  CHECK_BB((f = import_mfun(env, fun)))
/*  f->doc = "alloc other sizes for sinosc. also set phase (default: 2048)";*/
	CHECK_BB(import_class_end(env))
	return 1;
}


static struct Type_ t_gain      = { "Gain", 1, &t_ugen };
static m_bool gain_tick(UGen u)
{
  base_tick(u);
  u->out *= *(m_float*)u->ug;
}

static void gain_ctor(M_Object o, VM_Shred shred)
{
  assign_ugen(o->ugen, 1, 1, 0, malloc(sizeof(m_float)));
  o->ugen->tick = gain_tick;
  *(m_float*)o->ugen->ug = 1;
}

static void gain_dtor(M_Object o, VM_Shred shred)
{
  free(o->ugen->ug);
}

static void gain_get_gain(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  RETURN->v_float = *(m_float*)o->ugen->ug;
}

static void gain_set_gain(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  RETURN->v_float = *(m_float*)o->ugen->ug = *(m_float*)(shred->mem + SZ_FLOAT);
}

static m_bool import_gain(Env env)
{
  DL_Func* fun;
  CHECK_BB(add_global_type(env, &t_gain))
	CHECK_BB(import_class_begin(env, &t_gain, env->global_nspc, gain_ctor, gain_dtor))
	fun = new_DL_Func("float", "gain", (m_uint)gain_get_gain);
  CHECK_BB(import_mfun(env, fun))
  fun = new_DL_Func("float", "gain", (m_uint)gain_set_gain);
	  dl_func_add_arg(fun, "float", "arg0");
  CHECK_BB(import_mfun(env, fun))
	CHECK_BB(import_class_end(env))
  return 1;
}


static struct Type_ t_impulse      = { "Impulse", 1, &t_ugen };
static m_bool impulse_tick(UGen u)
{
  u->last = u->out = *(m_float*)u->ug;
	*(m_float*)u->ug = 0;
	u->done = 1;
}

static void impulse_ctor(M_Object o, VM_Shred shred)
{
  assign_ugen(o->ugen, 0, 1, 0, malloc(sizeof(m_float)));
  o->ugen->tick = impulse_tick;
  *(m_float*)o->ugen->ug = 0;
}

static void impulse_dtor(M_Object o, VM_Shred shred)
{
  free(o->ugen->ug);
}


static void impulse_get_next(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  RETURN->v_float = *(m_float*)o->ugen->ug;
}

static void impulse_set_next(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  RETURN->v_float = (*(m_float*)o->ugen->ug = *(m_float*)(shred->mem + SZ_INT));
}

static m_bool import_impulse(Env env)
{
  DL_Func* fun;
  CHECK_BB(add_global_type(env, &t_impulse))
  CHECK_BB(import_class_begin(env, &t_impulse, env->global_nspc, impulse_ctor, impulse_dtor))
  fun = new_DL_Func("float", "next", (m_uint)impulse_get_next);
  CHECK_BB(import_mfun(env, fun))
  fun = new_DL_Func("float", "next", (m_uint)impulse_set_next);
  dl_func_add_arg(fun, "float", "arg0");
  CHECK_BB(import_mfun(env, fun))
	CHECK_BB(import_class_end(env))
  return 1;
}

static struct Type_ t_fullrect = { "FullRect", 1, &t_ugen };
static m_bool fullrect_tick(UGen u)
{
  base_tick(u);
    u->out = fabs(u->in);
  return 1;
}

static void fullrect_ctor(M_Object o, VM_Shred shred)
{
  assign_ugen(o->ugen, 1, 1, 0, malloc(sizeof(m_float)));
  o->ugen->tick = fullrect_tick;
  *(m_float*)o->ugen->ug = 1;
}

static void fullrect_dtor(M_Object o, VM_Shred shred)
{
	free(o->ugen->ug);
}

static m_bool import_fullrect(Env env)
{
  DL_Func* fun;
	CHECK_BB(add_global_type(env, &t_fullrect))
	CHECK_BB(import_class_begin(env, &t_fullrect, env->global_nspc, fullrect_ctor, fullrect_dtor))
	CHECK_BB(import_class_end(env))
  return 1;
}

static struct Type_ t_halfrect = { "HalfRect", 1, &t_ugen };
static m_bool halfrect_tick(UGen u)
{
  base_tick(u);
  if(u->in > 0)
    u->out = u->in;
  else
    u->out = 0;
  return 1;
}

static void halfrect_ctor(M_Object o, VM_Shred shred)
{
  assign_ugen(o->ugen, 1, 1, 0, malloc(sizeof(m_float)));
  o->ugen->tick = halfrect_tick;
  *(m_float*)o->ugen->ug = 1;
}

static void halfrect_dtor(M_Object o, VM_Shred shred)
{
	free(o->ugen->ug);
}

static m_bool import_halfrect(Env env)
{
	CHECK_BB(add_global_type(env, &t_halfrect))
	CHECK_BB(import_class_begin(env, &t_halfrect, env->global_nspc, halfrect_ctor, halfrect_dtor))
	CHECK_BB(import_class_end(env))
  return 1;
}

static struct Type_ t_step = { "Step", 1, &t_ugen };
static m_bool step_tick(UGen u)
{
  u->out = *(m_float*)u->ug;
}

static void step_ctor(M_Object o, VM_Shred shred)
{
  assign_ugen(o->ugen, 0, 1, 0, malloc(sizeof(m_float)));
  o->ugen->tick = step_tick;
  *(m_float*)o->ugen->ug = 1;
}

static void step_dtor(M_Object o, VM_Shred shred)
{
  free(o->ugen->ug);
}


static void step_get_next(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  RETURN->v_float = *(m_float*)o->ugen->ug;
}

static void step_set_next(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
	printf("%f\n", *(m_float*)(shred->mem + SZ_INT));
  RETURN->v_float = *(m_float*)o->ugen->ug = *(m_float*)(shred->mem + SZ_INT);
}

static m_bool import_step(Env env)
{
  DL_Func* fun;
	CHECK_BB(add_global_type(env, &t_step))
	CHECK_BB(import_class_begin(env, &t_step, env->global_nspc, step_ctor, step_dtor))
  fun = new_DL_Func("float", "next", (m_uint)step_get_next);
  CHECK_BB(import_mfun(env, fun))
  fun = new_DL_Func("float", "next", (m_uint)step_set_next);
  dl_func_add_arg(fun, "float", "arg0");
  CHECK_BB(import_mfun(env, fun))
	CHECK_BB(import_class_end(env))
  return 1;
}

static struct Type_ t_zerox       = { "ZeroX",       1, &t_ugen };

static m_bool zerox_tick(UGen u)
{
  base_tick(u);
  u->out = 0;
  if(!u->in)
  {
    if(*(m_float*)u->ug < 0)
      u->out = 1;
    else if(*(m_float*)u->ug < 1)
      u->out = -1;
  }
  *(m_float*)u->ug = u->in;
}

static void zerox_ctor(M_Object o, VM_Shred shred)
{
  assign_ugen(o->ugen, 1, 1, 0, malloc(sizeof(m_float)));
  o->ugen->tick = zerox_tick;
  *(m_float*)o->ugen->ug = 1;
}

static void zerox_dtor(M_Object o, VM_Shred shred)
{
  free(o->ugen->ug);
}

static m_bool import_zerox(Env env)
{
  DL_Func* fun;
	CHECK_BB(add_global_type(env, &t_zerox))
	CHECK_BB(import_class_begin(env, &t_zerox, env->global_nspc, zerox_ctor, zerox_dtor))
	CHECK_BB(import_class_end(env))
  return 1;
}

m_bool import_modules(Env env)
{
  CHECK_BB(import_sinosc(env))
  CHECK_BB(import_gain(env))
  CHECK_BB(import_impulse(env))
  CHECK_BB(import_fullrect(env))
  CHECK_BB(import_halfrect(env))
  CHECK_BB(import_step(env))
  CHECK_BB(import_zerox(env))
	return 1;
}
