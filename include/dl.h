#ifndef DL_H
#define DL_H

#include "defs.h"
#include "vm.h"
//#include "type.h"

#define CK_DLL_CALL
#include <stdlib.h>

typedef struct DL_Return
{
	union
	{
	  m_uint   v_uint;
	  m_float  v_float;
		complex  v_complex;
		VEC3_T 	 v_vec3;
		VEC4_T 	 v_vec4;
	  M_Object v_object;
	};
	m_uint offset;
} DL_Return;

//static void dl_return_push(const DL_Return retval, VM_Shred shred, Kindof kind);
void dl_return_push(const DL_Return retval, VM_Shred shred, int kind);

typedef void (CK_DLL_CALL * f_ctor)(M_Object o, VM_Shred sh);
typedef void (CK_DLL_CALL * f_dtor)(M_Object o, VM_Shred sh);
typedef void (CK_DLL_CALL * f_mfun)(M_Object o, DL_Return * RETURN, VM_Shred sh);
typedef void (CK_DLL_CALL * f_sfun)(DL_Return * RETURN, VM_Shred sh);
typedef void (CK_DLL_CALL * f_sfun)(DL_Return * RETURN, VM_Shred sh);
typedef m_bool (CK_DLL_CALL * f_init)(Env env);

typedef struct
{
  m_str name;
  m_str type;
  m_bool is_const;
  void * static_addr;  
  m_str doc;
} DL_Value;

DL_Value* new_DL_Value(const m_str t, const m_str  n, m_bool c, void* addr);

typedef struct 
{
  m_str name;
  m_str type;
  union { f_ctor ctor; f_dtor dtor; f_mfun mfun; f_sfun sfun; m_uint addr; };
  Vector args;
//  m_str doc;
} DL_Func;


DL_Func* new_DL_Func(const m_str t, const m_str n, m_uint addr);

void freeDL_Func(DL_Func* a);
DL_Value* dl_func_add_arg(DL_Func* a, const m_str t, const m_str  n);
#endif
