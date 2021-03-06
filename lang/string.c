#include "defs.h"
#include "vm.h"
#include "type.h"
#include "err_msg.h"
#include "import.h"
#include "instr.h"

struct Type_ t_string     = { "string",     sizeof(m_uint), &t_object, te_string};
m_int o_string_data;

static void String_Assign(VM * vm, VM_Shred shred, Instr instr)
{
#ifdef DEBUG_INSTR
  debug_msg("instr", "string => string");
#endif
  shred->reg -= SZ_INT * 2;
  M_Object lhs = *(M_Object*)shred->reg;
  if(!lhs)
  {
    lhs = new_M_Object();
    initialize_object(lhs, &t_string);
  }
  M_Object rhs = *(M_Object*)(shred->reg + SZ_INT);
  if(!rhs)
  {
    rhs = new_M_Object();
    initialize_object(rhs, &t_string);
  }
  STRING(rhs) = STRING(lhs);
  *(M_Object*)shred->reg = rhs;
  shred->reg += SZ_INT;  
}

static void Int_String_Assign(VM * vm, VM_Shred shred, Instr instr)
{
#ifdef DEBUG_INSTR
  debug_msg("instr", "int '=>' string");
#endif
  shred->reg -= SZ_INT * 2;
  m_int lhs = *(m_int*)shred->reg;
  M_Object rhs = *(M_Object*)(shred->reg + SZ_INT);
  m_str str;
  sprintf(str, "%i", lhs);
  STRING(rhs) = str;
  *(M_Object*)shred->reg =  (M_Object)rhs;
  shred->reg += SZ_INT;
}

static void Float_String_Assign(VM * vm, VM_Shred shred, Instr instr)
{
#ifdef DEBUG_INSTR
  debug_msg("instr", "float '=>' string");
#endif
  shred->reg -= SZ_INT * 2;
  m_float lhs = *(m_float*)shred->reg;
  M_Object rhs = *(M_Object*)(shred->reg + SZ_FLOAT);
  m_str str;
  sprintf(str, "%f", lhs);
  STRING(rhs) = str;
  *(M_Object*)shred->reg = (M_Object)rhs;
  shred->reg += SZ_INT;
}

INSTR(Complex_String_Assign)
{
#ifdef DEBUG_INSTR
  debug_msg("instr", "Complex '=>' string");
#endif
  shred->reg -= SZ_INT * 2;
  complex lhs = *(complex*)shred->reg;
  M_Object rhs = *(M_Object*)(shred->reg + SZ_INT);
  m_str str;
  sprintf(str, "#(%f, %f)", creal(lhs), cimag(lhs));
  STRING(rhs) = str;
  *(M_Object*)shred->reg = (M_Object)rhs;
  shred->reg += SZ_INT;
}
INSTR(Object_String_Assign)
{
#ifdef DEBUG_INSTR
  debug_msg("instr", "Object '=>' string");
#endif
  shred->reg -= SZ_INT * 2;
  M_Object lhs = *(M_Object*)shred->reg;
  M_Object rhs = *(M_Object*)(shred->reg + SZ_INT);
  m_str str;
  sprintf(str, "%p", lhs);
  STRING(rhs) = str;
  *(M_Object*)shred->reg = (M_Object)rhs;
  shred->reg += SZ_INT;
}

INSTR(String_String)
{
#ifdef DEBUG_INSTR
  debug_msg("instr", "int '+' string");
#endif
  shred->reg -= SZ_INT * 2;
  M_Object lhs = *(M_Object*)shred->reg;
  M_Object rhs = *(M_Object*)(shred->reg + SZ_INT);
  m_str str;
  sprintf(str, "%s%s", STRING(lhs), STRING(rhs));
  *(M_Object*)shred->reg = new_String(str);
  shred->reg += SZ_INT;
}

INSTR(Int_String)
{
#ifdef DEBUG_INSTR
  debug_msg("instr", "int '+' string");
#endif
  shred->reg -= SZ_INT * 2;
  m_int lhs = *(m_int*)shred->reg;
  M_Object rhs = *(M_Object*)(shred->reg + SZ_INT);
  m_str str;
  sprintf(str, "%i%s", lhs, rhs);
  *(M_Object*)shred->reg = new_String(str);
  shred->reg += SZ_INT;
}

INSTR(Float_String)
{
#ifdef DEBUG_INSTR
  debug_msg("instr", "int '+' string");
#endif
  shred->reg -= SZ_INT * 2;
  m_float lhs = *(m_float*)shred->reg;
  M_Object rhs = *(M_Object*)(shred->reg + SZ_INT);
  m_str str;
  sprintf(str, "%f%s", lhs, rhs);
  *(M_Object*)shred->reg = new_String(str);
  shred->reg += SZ_INT;
}

INSTR(Complex_String)
{
#ifdef DEBUG_INSTR
  debug_msg("instr", "int '+' string");
#endif
  shred->reg -= SZ_INT * 2;
  M_Object lhs = *(M_Object*)shred->reg;
  M_Object rhs = *(M_Object*)(shred->reg + SZ_INT);
  m_str str;
  sprintf(str, "%p%s", lhs, rhs);
  *(M_Object*)shred->reg = new_String(str);
  shred->reg += SZ_INT;
}

INSTR(Object_String)
{
#ifdef DEBUG_INSTR
  debug_msg("instr", "int '+' string");
#endif
  shred->reg -= SZ_INT * 2;
  m_float lhs = *(m_float*)shred->reg;
  M_Object rhs = *(M_Object*)(shred->reg + SZ_INT);
  m_str str;
  sprintf(str, "#(%f, %f)%s", creal(lhs), cimag(lhs), rhs);
  *(M_Object*)shred->reg = new_String(str);
  shred->reg += SZ_INT;
}

INSTR(String_Plus)
{
#ifdef DEBUG_INSTR 
  debug_msg("instr", "string '+=>' string");
#endif
  shred->reg -= SZ_INT * 2;
  M_Object lhs = *(M_Object*)shred->reg;
  M_Object rhs = **(M_Object**)(shred->reg + SZ_INT);
  m_uint l_len = strlen(STRING(lhs));
  m_uint r_len = strlen(STRING(rhs));
  char c[l_len + r_len];
  sprintf(c, "%s%s", STRING(rhs), STRING(lhs));
/*  free(STRING(rhs));*/
  STRING(rhs) = strdup(c);
/*  STRING(rhs) = c;*/
  *(M_Object*)shred->reg = rhs;
  shred->reg += SZ_INT;
}

INSTR(Int_String_Plus)
{
#ifdef DEBUG_INSTR 
  debug_msg("instr", "int '+=>' string");
#endif
  shred->reg -= SZ_INT * 2;
  m_uint lhs = *(m_uint*)shred->reg;
  M_Object rhs = **(M_Object**)(shred->reg + SZ_INT);
  m_uint len = strlen(STRING(rhs)) + 1;
  m_uint tmp = lhs;
  while(tmp /= 10)
    len++;
  char c[len];
  sprintf(c, "%s%i", STRING(rhs), lhs);
/*  free(STRING(rhs));*/
  STRING(rhs) = strdup(c);
  *(M_Object*)shred->reg = rhs;
  shred->reg += SZ_INT;
}

INSTR(Float_String_Plus)
{
#ifdef DEBUG_INSTR 
  debug_msg("instr", "float '+=>' string");
#endif
  shred->reg -= SZ_INT * 2;
  m_float lhs = *(m_float*)shred->reg;
  M_Object rhs = **(M_Object**)(shred->reg + SZ_INT);
  m_uint len = strlen(STRING(rhs)) + 1 + 7;
  m_uint tmp = lhs;
  while(tmp /= 10)
    len++;
  char c[len];
  sprintf(c, "%s%f", STRING(rhs), lhs);
/*  free(STRING(rhs));*/
  STRING(rhs) = strdup(c);
  *(M_Object*)shred->reg = rhs;
  shred->reg += SZ_INT;
}

INSTR(Complex_String_Plus)
{
#ifdef DEBUG_INSTR 
  debug_msg("instr", "float '+=>' string");
#endif
  shred->reg -= SZ_INT * 2;
  m_float lhs = *(m_float*)shred->reg;
  M_Object rhs = **(M_Object**)(shred->reg + SZ_INT);
  m_uint len = strlen(STRING(rhs)) + 1 + 5 + 13;
  m_uint tmp = creal(lhs);
  while(tmp /= 10)
    len++;
  tmp = cimag(lhs);
  while(tmp /= 10)
    len++;
  char c[len];
  sprintf(c, "%s#(%f, %f)", STRING(rhs), lhs);
/*  free(STRING(rhs));*/
  STRING(rhs) = strdup(c);
  *(M_Object*)shred->reg = rhs;
  shred->reg += SZ_INT;
}

INSTR(Object_String_Plus)
{
#ifdef DEBUG_INSTR 
  debug_msg("instr", "string '+=>' string");
#endif
  shred->reg -= SZ_INT * 2;
  M_Object lhs = *(M_Object*)shred->reg;
  M_Object rhs = **(M_Object**)(shred->reg + SZ_INT);
  m_uint len = strlen(STRING(rhs)) + 9;
  char c[len];
  sprintf(c, "%s%p", STRING(rhs), lhs);
/*  free(STRING(rhs));*/
  STRING(rhs) = strdup(c);
/*  STRING(rhs) = c;*/
  *(M_Object*)shred->reg = rhs;
  shred->reg += SZ_INT;
}
void Reg_Push_Str(VM* vm, VM_Shred shred, Instr instr)
{
#ifdef DEBUG_INSTR 
  debug_msg("instr", "push string %s", (m_str)instr->m_val);
#endif
  *(M_Object*)shred->reg = new_String((m_str)instr->m_val);
  shred->reg += SZ_INT;
#ifdef DEBUG_INSTR 
  debug_msg("instr", "push string");
#endif
}

void string_ctor(M_Object o, VM_Shred shred)
{
  STRING(o) = "this is the default string";
}

void string_len(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
	RETURN->v_uint = strlen(STRING(o));
}

void string_upper(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
	m_uint i;
	M_Object obj = new_M_Object();
  initialize_object(obj, &t_string);
	m_str str = STRING(obj) = strdup(STRING(o));
	for(i = 0; i < strlen(str); i++)
		if(str[i]  >= 'a' && str[i] <= 'z')
    	str[i] += 'A' - 'a';
	RETURN->v_object = obj;
}

void string_lower(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
	m_uint i;
	M_Object obj = new_M_Object();
  initialize_object(obj, &t_string);
	m_str str = STRING(obj) = strdup(STRING(o));
	for(i = 0; i < strlen(str); i++)
		if(str[i]  >= 'A' && str[i] <= 'Z')
    	str[i] -= 'A' - 'a';
	RETURN->v_object = obj;
}

void string_ltrim(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
	m_uint i = 0;
	M_Object obj = new_M_Object();
  initialize_object(obj, &t_string);
	m_str str = STRING(obj) = strdup(STRING(o));
	while(*str || *str == ' ')
	{ i++; str++; }
	RETURN->v_object = obj;
}

void string_rtrim(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
	M_Object obj = new_M_Object();
  initialize_object(obj, &t_string);
	m_str str = STRING(obj) = strdup(STRING(o));
	RETURN->v_object = obj;
}

void string_trim(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_str str = STRING(o);
  m_int i, start = 0, end = 0, len = 0;
  M_Object obj = new_M_Object();
  initialize_object(obj, &t_string);
  STRING(obj) = str;
  while(str[len] != '\0')
    len++;
  for(i = 0; i < len; i++)
  {
    if(str[i] == ' ')
      start++;
    else break;
  }
/*  exit(2);*/
  for(i = len -1; i >= 0; i--)
  {
    if(str[i] == ' ')
      end++;
    else break;
  }
  char c[len - start - end + 1];
  for(i = start; i < len - end; i++)
    c[i-start] = str[i];
  c[len - start -end ] = '\0';
  STRING(obj) = strdup(c);
  RETURN->v_object = obj;
}

void string_charAt(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_str str = STRING(o);
  m_int i = *(m_int*)(shred->mem + SZ_INT), len = 0;
  while(str[len] != '\0')
    len++;
  if(i < 0 || i >= len)
    RETURN->v_uint = -1;
  else
    RETURN->v_uint = str[i];
}


void string_setCharAt(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_str str = STRING(o);
  m_int i = *(m_int*)(shred->mem + SZ_INT), len = 0;
  m_int c = *(m_int*)(shred->mem + SZ_INT * 2);
  while(str[len] != '\0')
    len++;
  if(i < 0 || i >= len)
    RETURN->v_uint = -1;
  else
  {
    str[i] = c;
    STRING(o) = strdup(str);
    RETURN->v_uint = c;
  }
}

void string_substring(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_str str = strdup(STRING(o));
  m_int i, len = 0, index = *(m_int*)(shred->mem + SZ_INT);
  M_Object obj = new_M_Object();
  initialize_object(obj, &t_string);
  STRING(obj) = str;
  while(str[len] != '\0')
    len++;
  char c[len - index];
  for(i = index; i < len; i++)
    c[i-index] = str[i];
  STRING(obj) = strdup(c);
  RETURN->v_object = obj;
}

void string_substringN(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_str str = strdup(STRING(o));
  m_int i, len = 0, index = *(m_int*)(shred->mem + SZ_INT * 2);
  m_int end = *(m_int*)(shred->mem + SZ_INT);
  M_Object obj = new_M_Object();
  initialize_object(obj, &t_string);
  STRING(obj) = str;
  while(str[len] != '\0')
    len++;
  len -= end;
  char c[len - index +1];
  for(i = index; i < len; i++)
    c[i-index] = str[i];
  c[i-index] = '\0';
  STRING(obj) = strdup(c);
  RETURN->v_object = obj;
}

void string_insert(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_str str = strdup(STRING(o));
  m_int i, len = 0, len_insert = 0, index = *(m_int*)(shred->mem + SZ_INT);
  M_Object arg = *(M_Object*)(shred->mem + SZ_INT * 2);
  m_str insert = strdup(STRING(arg));
  M_Object obj = new_M_Object();
  initialize_object(obj, &t_string);
  while(str[len] != '\0')
    len++;
  len_insert =  strlen(insert);
  char c[len + len_insert + 1];
  for(i = 0; i < index; i++)
    c[i] = str[i];
  for(i = 0; i < len_insert; i++)
    c[i + index] = insert[i];
  for(i = index; i < len; i++)
    c[i + len_insert] = str[i];
  c[len + len_insert] = '\0';
  STRING(obj) = strdup(c);
  RETURN->v_object = obj;
}

void string_replace(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_str str = strdup(STRING(o));
  m_int i, len = 0, len_insert = 0, index = *(m_int*)(shred->mem + SZ_INT);
  M_Object arg = *(M_Object*)(shred->mem + SZ_INT * 2);
  m_str insert = strdup(STRING(arg));
  M_Object obj = new_M_Object();
  initialize_object(obj, &t_string);
  while(str[len] != '\0')
    len++;
  len_insert =  strlen(insert);
  char c[index + len_insert + 1];
  for(i = 0; i < index; i++)
    c[i] = str[i];
  for(i = 0; i < len_insert; i++)
    c[i + index] = insert[i];
  c[index + len_insert] = '\0';
  STRING(obj) = strdup(c);
  RETURN->v_object = obj;
}

void string_replaceN(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_str str = strdup(STRING(o));
  m_int i, len = 0, index = *(m_int*)(shred->mem + SZ_INT);
  M_Object arg = *(M_Object*)(shred->mem + SZ_INT * 3);
  m_int _len = *(m_int*)(shred->mem + SZ_INT * 2);
  m_str insert = strdup(STRING(arg));
  M_Object obj = new_M_Object();
  initialize_object(obj, &t_string);
  while(str[len] != '\0')
    len++;
  len = len > index + _len ? len : index + _len;
  char c[len + 1];
  for(i = 0; i < index; i++)
    c[i] = str[i];
  for(i = 0; i < _len; i++)
    c[i + index] = insert[i];
  for(i = index + _len; i < len; i++)
    c[i] = str[i];
  c[len] = '\0';
  STRING(obj) = strdup(c);
  RETURN->v_object = obj;
}

void string_find(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_str str = STRING(o);
  m_int i = 0, ret = -1;
  char arg = *(m_int*)(shred->mem + SZ_INT);
  while(str[i] != '\0')
  {
    if(str[i] == arg)
    {
      ret = i;
      break;
    }
    i++;
  }
  RETURN->v_uint = ret;
}

void string_findStart(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_str str = STRING(o);
  char pos = *(m_int*)(shred->mem + SZ_INT);
  char arg = *(m_int*)(shred->mem + SZ_INT * 2);
  m_int i = pos, ret = -1;
  while(str[i] != '\0')
  {
    if(str[i] == arg)
    {
      ret = i;
      break;
    }
    i++;
  }
  RETURN->v_uint = ret;
}

void string_findStr(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_str str = strdup(STRING(o));
  m_int ret = -1;
  M_Object obj = *(M_Object*)(shred->mem + SZ_INT);
  m_str arg = STRING(obj);
  m_int len  = strlen(str);
  m_int i = 0;
  m_int arg_len = strlen(arg);
  while(i < len)
  {
    if(!strncmp(str, arg, arg_len))
    {
      ret = i;
      break;
    }
    i++;
    str++;
  }
  RETURN->v_uint = ret;
}

void string_findStrStart(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_str str = strdup(STRING(o));
  m_int ret = -1;
  m_int start = *(m_int*)(shred->mem + SZ_INT);
  M_Object obj = *(M_Object*)(shred->mem + SZ_INT * 2);
  m_str arg = STRING(obj);
  m_int len  = strlen(str);
  m_int i = start;
  m_int arg_len = strlen(arg);
  while(i < len)
  {
    if(!strncmp(str, arg, arg_len))
    {
      ret = i;
      break;
    }
    i++;
    str++;
  }
  RETURN->v_uint = ret;
}

void string_rfind(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_str str = STRING(o);
  m_int i = strlen(str), ret = -1;
  char arg = *(m_int*)(shred->mem + SZ_INT);
  while(str[i] != '\0')
  {
    if(str[i] == arg)
    {
      ret = i;
      break;
    }
    i--;
  }
  RETURN->v_uint = ret;
}

void string_rfindStart(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_str str = STRING(o);
  char pos = *(m_int*)(shred->mem + SZ_INT);
  char arg = *(m_int*)(shred->mem + SZ_INT * 2);
  m_int i = pos, ret = -1;
  while(str[i] != '\0')
  {
    if(str[i] == arg)
    {
      ret = i;
      break;
    }
    i--;
  }
  RETURN->v_uint = ret;
}

void string_rfindStr(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_str str = strdup(STRING(o));
  m_int ret = -1;
  M_Object obj = *(M_Object*)(shred->mem + SZ_INT);
  m_str arg = STRING(o);
  m_int len  = strlen(str);
  m_int i = len;
  m_int arg_len = strlen(arg);
  str += len -1;
  while(i)
  {
    if(!strncmp(str, arg, arg_len))
    {
      ret = i;
      break;
    }
    i--;
    str--;
  }
  RETURN->v_uint = ret;
}

void string_rfindStrStart(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_str str = strdup(STRING(o));
  m_int ret = -1;
  m_int start = *(m_int*)(shred->mem + SZ_INT);
  M_Object obj = *(M_Object*)(shred->mem + SZ_INT * 2);
  m_str arg = STRING(o);
  m_int len  = strlen(str);
  m_int i = start;
  m_int arg_len = strlen(arg);
  str += len -1;
  while(i < len)
  {
    if(!strncmp(str, arg, arg_len))
    {
      ret = i;
      break;
    }
    i--;
    str--;
  }
  RETURN->v_uint = ret;
}

void string_erase(M_Object o, DL_Return * RETURN, VM_Shred shred)
{
  m_str str = STRING(o);
  m_int i;
  m_int start = *(m_int*)(shred->mem + SZ_INT);
  m_int rem = *(m_int*)(shred->mem + SZ_INT * 2);
  m_int len = strlen(str);
  char c[len - rem];
  for(i = 0; i < start; i++)
    c[i] = str[i];
  for(i = start + rem; i < len; i++)
    c[i -rem] = str[i];
}

void string_toInt(M_Object o, DL_Return * RETURN, VM_Shred shred)
{ RETURN->v_uint = atoi(STRING(o)); }

void string_toFloat(M_Object o, DL_Return * RETURN, VM_Shred shred)
{  RETURN->v_float = atof(STRING(o));	}

m_bool import_string(Env env)
{
  CHECK_BB(add_global_type(env, &t_string));
  CHECK_BB(import_class_begin(env, &t_string, env->global_nspc, string_ctor, NULL))
	env->class_def->doc = "chain of characters";

  o_string_data = import_mvar(env, "int", "@data",   1, 0, "place to hold the string");
  
/*  import_svar(env, "int", "trs",   1, malloc(sizeof(m_uint)), "place to hold the string");*/
/*exit(2);*/
  CHECK_BB(o_string_data)
  DL_Func* fun = new_DL_Func("int", "size", (m_uint)string_len);
  CHECK_OB(import_mfun(env, fun))

  fun = new_DL_Func("string", "upper", (m_uint)string_upper);
  CHECK_OB(import_mfun(env, fun))

  fun = new_DL_Func("string", "lower", (m_uint)string_lower);
  CHECK_OB(import_mfun(env, fun))

  fun = new_DL_Func("string", "ltrim", (m_uint)string_ltrim);
  CHECK_OB(import_mfun(env, fun))

  fun = new_DL_Func("string", "rtrim", (m_uint)string_rtrim);
  CHECK_OB(import_mfun(env, fun))

  fun = new_DL_Func("string", "trim", (m_uint)string_trim);
  CHECK_OB(import_mfun(env, fun))

  fun = new_DL_Func("int", "charAt", (m_uint)string_charAt);
  dl_func_add_arg(fun, "int", "pos");
  CHECK_OB(import_mfun(env, fun))
  
  fun = new_DL_Func("int", "charAt", (m_uint)string_setCharAt);
  dl_func_add_arg(fun, "int", "pos");
  dl_func_add_arg(fun, "int", "char");
  CHECK_OB(import_mfun(env, fun))
  
  fun = new_DL_Func("string", "substring", (m_uint)string_substring);
  dl_func_add_arg(fun, "int", "start");
  CHECK_OB(import_mfun(env, fun))
  
  fun = new_DL_Func("string", "substring", (m_uint)string_substringN);
  dl_func_add_arg(fun, "int", "start");
  dl_func_add_arg(fun, "int", "end");
  CHECK_OB(import_mfun(env, fun))
  
  fun = new_DL_Func("string", "insert", (m_uint)string_insert);
  dl_func_add_arg(fun, "int", "pos");
  dl_func_add_arg(fun, "string", "string");
  CHECK_OB(import_mfun(env, fun))
  
  fun = new_DL_Func("string", "replace", (m_uint)string_replace);
  dl_func_add_arg(fun, "int", "pos");
  dl_func_add_arg(fun, "string", "string");
  CHECK_OB(import_mfun(env, fun))

  fun = new_DL_Func("string", "replace", (m_uint)string_replaceN);
  dl_func_add_arg(fun, "int", "pos");
  dl_func_add_arg(fun, "int", "n");
  dl_func_add_arg(fun, "string", "string");
  CHECK_OB(import_mfun(env, fun))
  
  fun = new_DL_Func("int", "find", (m_uint)string_find);
  dl_func_add_arg(fun, "int", "char");
  CHECK_OB(import_mfun(env, fun))

  fun = new_DL_Func("int", "find", (m_uint)string_findStart);
  dl_func_add_arg(fun, "int", "pos");
  dl_func_add_arg(fun, "int", "char");
  CHECK_OB(import_mfun(env, fun))
  
  fun = new_DL_Func("int", "find", (m_uint)string_findStr);
  dl_func_add_arg(fun, "string", "str");
  CHECK_OB(import_mfun(env, fun))
  
  fun = new_DL_Func("int", "find", (m_uint)string_findStrStart);
  dl_func_add_arg(fun, "int", "pos");
  dl_func_add_arg(fun, "string", "str");
  CHECK_OB(import_mfun(env, fun))  
  
   fun = new_DL_Func("int", "rfind", (m_uint)string_rfind);
  dl_func_add_arg(fun, "int", "char");
  CHECK_OB(import_mfun(env, fun))

  fun = new_DL_Func("int", "rfind", (m_uint)string_rfindStart);
  dl_func_add_arg(fun, "int", "pos");
  dl_func_add_arg(fun, "int", "char");
  CHECK_OB(import_mfun(env, fun))
  
  fun = new_DL_Func("int", "rfind", (m_uint)string_rfindStr);
  dl_func_add_arg(fun, "string", "str");
  CHECK_OB(import_mfun(env, fun))
  
  fun = new_DL_Func("int", "rfind", (m_uint)string_rfindStrStart);
  dl_func_add_arg(fun, "int", "pos");
  dl_func_add_arg(fun, "string", "str");
  CHECK_OB(import_mfun(env, fun))  

  fun = new_DL_Func("void",   "erase", (m_uint)string_erase);
  dl_func_add_arg(fun, "int", "start");
  dl_func_add_arg(fun, "int", "length");
  CHECK_OB(import_mfun(env, fun)) 
  
  fun = new_DL_Func("int", "toInt", (m_uint)string_toInt);
  CHECK_OB(import_mfun(env, fun))
  
  fun = new_DL_Func("float", "toFloat", (m_uint)string_toFloat);
  CHECK_OB(import_mfun(env, fun))

  CHECK_BB(add_binary_op(env, op_chuck, &t_string,  &t_string, &t_string, String_Assign, 1))
	CHECK_BB(add_binary_op(env, op_chuck, &t_int,     &t_string, &t_string, Int_String_Assign, 1))
	CHECK_BB(add_binary_op(env, op_chuck, &t_float,   &t_string, &t_string, Float_String_Assign, 1))
	CHECK_BB(add_binary_op(env, op_chuck, &t_complex, &t_string, &t_string, Complex_String_Assign, 1))
	CHECK_BB(add_binary_op(env, op_chuck, &t_object,  &t_string, &t_string, Object_String_Assign, 1))

  CHECK_BB(add_binary_op(env, op_plus, &t_string,  &t_string, &t_string, String_String, 1))
	CHECK_BB(add_binary_op(env, op_plus, &t_int,     &t_string, &t_string, Int_String, 1))
	CHECK_BB(add_binary_op(env, op_plus, &t_float,   &t_string, &t_string, Float_String, 1))
	CHECK_BB(add_binary_op(env, op_plus, &t_complex, &t_string, &t_string, Complex_String, 1))
	CHECK_BB(add_binary_op(env, op_plus, &t_object,  &t_string, &t_string, Object_String, 1))

  CHECK_BB(add_binary_op(env, op_plus_chuck, &t_string,  &t_string, &t_string, String_Plus, 1))
	CHECK_BB(add_binary_op(env, op_plus_chuck, &t_int,     &t_string, &t_string, Int_String_Plus, 1))
	CHECK_BB(add_binary_op(env, op_plus_chuck, &t_float,   &t_string, &t_string, Float_String_Plus, 1))
	CHECK_BB(add_binary_op(env, op_plus_chuck, &t_complex, &t_string, &t_string, Complex_String_Plus, 1))
	CHECK_BB(add_binary_op(env, op_plus_chuck, &t_object,  &t_string, &t_string, Object_String_Plus, 1))
  CHECK_BB(import_class_end(env))
  return 1;
}
