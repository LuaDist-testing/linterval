/*
* linterval.c
* interval arithmetic library for Lua 5.1 based on BIAS
* Luiz Henrique de Figueiredo <lhf@tecgraf.puc-rio.br>
* 01 May 2012 17:52:02
* This code is hereby placed in the public domain.
*/

#include "BIAS/Bias0.h"
#include "BIAS/BiasF.h"

#include "lua.h"
#include "lauxlib.h"

#define MYNAME		"interval"
#define MYVERSION	MYNAME " library for " LUA_VERSION " / May 2012 / "\
			"using BIAS"
#define MYTYPE		MYNAME

typedef BIASINTERVAL *Interval;
typedef const BIASINTERVAL *CInterval;

static Interval Pnew(lua_State *L)
{
 Interval x=lua_newuserdata(L,sizeof(BIASINTERVAL));
 luaL_getmetatable(L,MYTYPE);
 lua_setmetatable(L,-2);
 return x;
}

static Interval Pget(lua_State *L, int i, Interval x)
{
 switch (lua_type(L,i))
 {
  case LUA_TNUMBER:
  case LUA_TSTRING:
  {
   REAL a=lua_tonumber(L,i);
   BiasHullR(x,&a);
   return x;
  }
  default:
   return luaL_checkudata(L,i,MYTYPE);
 }
 return NULL;
}

static int Ltype(lua_State *L)			/** type(x) */
{
 luaL_checkudata(L,1,MYTYPE);
 lua_pushliteral(L,MYTYPE);
 return 1;
}

static int Lnew(lua_State *L)			/** new(a,b) */
{
 REAL a=luaL_checknumber(L,1);
 REAL b=luaL_optnumber(L,2,a);
 Interval x=Pnew(L);
 BiasHullRR(x,&a,&b);
 return 1;
}

static int Pdo1(lua_State *L, void (*f)(Interval r, CInterval a))
{
 BIASINTERVAL A;
 Interval a=Pget(L,1,&A);
 Interval r=Pnew(L);
 f(r,a);
 return 1;
}

static int Pdo2(lua_State *L, void (*f)(Interval r, CInterval a, CInterval b))
{
 BIASINTERVAL A,B;
 Interval a=Pget(L,1,&A);
 Interval b=Pget(L,2,&B);
 Interval r=Pnew(L);
 f(r,a,b);
 return 1;
}

static int Lextremes(lua_State *L)		/** extremes(x) */
{
 BIASINTERVAL A;
 Interval a=Pget(L,1,&A);
 lua_pushnumber(L,BiasInf(a));
 lua_pushnumber(L,BiasSup(a));
 return 2;
}

static int Lmidrad(lua_State *L)		/** midrad(x) */
{
 BIASINTERVAL A;
 Interval a=Pget(L,1,&A);
 REAL m,r;
 BiasMidRad(&m,&r,a);
 lua_pushnumber(L,m);
 lua_pushnumber(L,r);
 return 2;
}

static int Ldiam(lua_State *L)			/** diam(x) */
{
 BIASINTERVAL A;
 Interval a=Pget(L,1,&A);
 lua_pushnumber(L,BiasDiam(a));
 return 1;
}

static int Linf(lua_State *L)			/** inf(x) */
{
 BIASINTERVAL A;
 Interval a=Pget(L,1,&A);
 lua_pushnumber(L,BiasInf(a));
 return 1;
}

static int Lsup(lua_State *L)			/** sup(x) */
{
 BIASINTERVAL A;
 Interval a=Pget(L,1,&A);
 lua_pushnumber(L,BiasSup(a));
 return 1;
}

static int Lmid(lua_State *L)			/** mid(x) */
{
 BIASINTERVAL A;
 Interval a=Pget(L,1,&A);
 lua_pushnumber(L,BiasMid(a));
 return 1;
}

static int Lipow(lua_State *L)			/** ipow(x,n) */
{
 BIASINTERVAL A;
 Interval a=Pget(L,1,&A);
 INT n=luaL_checkinteger(L,2);
 Interval r=Pnew(L);
 BiasPowerN(r,a,n);
 return 1;
}

static int Lroot(lua_State *L)			/** root(x,n) */
{
 BIASINTERVAL A;
 Interval a=Pget(L,1,&A);
 INT n=luaL_checkinteger(L,2);
 Interval r=Pnew(L);
 BiasRoot(r,a,n);
 return 1;
}

static int Lcontains(lua_State *L)		/** contains(x,t) */
{
 BIASINTERVAL A;
 Interval a=Pget(L,1,&A);
 REAL t=luaL_checknumber(L,2);
 lua_pushboolean(L,BiasInR(&t,a));
 return 1;
}

static int Lcontained(lua_State *L)		/** contained(x,y) */
{
 BIASINTERVAL A,B;
 Interval a=Pget(L,1,&A);
 Interval b=Pget(L,2,&B);
 lua_pushboolean(L,BiasInI(a,b));
 return 1;
}

static int Llt(lua_State *L)
{
 BIASINTERVAL A,B;
 Interval a=Pget(L,1,&A);
 Interval b=Pget(L,2,&B);
 lua_pushboolean(L,BiasSup(a)<BiasInf(b));
 return 1;
}

static int Lle(lua_State *L)
{
 BIASINTERVAL A,B;
 Interval a=Pget(L,1,&A);
 Interval b=Pget(L,2,&B);
 lua_pushboolean(L,BiasSup(a)<=BiasInf(b));
 return 1;
}

static int Lmeet(lua_State *L)			/** meet(x,y) */
{
 BIASINTERVAL A,B;
 Interval a=Pget(L,1,&A);
 Interval b=Pget(L,2,&B);
 Interval r=Pnew(L);
 return BiasIntersection(r,a,b);
}

static int Ltostring(lua_State *L)		/** tostring(x) */
{
 BIASINTERVAL A;
 Interval a=Pget(L,1,&A);
 lua_pushfstring(L,"%s %p",MYTYPE,(void*)a);
 return 1;
}

#define DO(d,f,F)	static int L##f(lua_State *L) { return d(L,F); }
#define DO1(f,F)	DO(Pdo1,f,F)
#define DO2(f,F)	DO(Pdo2,f,F)

DO1(abs,BiasIAbs)				/** abs(x) */
DO1(acos,BiasArcCos)				/** acos(x) */
DO1(acosh,BiasArCosh)				/** acosh(x) */
DO1(acot,BiasArcCot)				/** acot(x) */
DO1(acoth,BiasArCoth)				/** acoth(x) */
DO1(asin,BiasArcSin)				/** asin(x) */
DO1(asinh,BiasArSinh)				/** asinh(x) */
DO1(atan,BiasArcTan)				/** atan(x) */
DO1(atanh,BiasArTanh)				/** atanh(x) */
DO1(cos,BiasCos)				/** cos(x) */
DO1(cosh,BiasCosh)				/** cosh(x) */
DO1(cot,BiasCot)				/** cot(x) */
DO1(coth,BiasCoth)				/** coth(x) */
DO1(exp,BiasExp)				/** exp(x) */
DO1(log10,BiasLog10)				/** log10(x) */
DO1(log,BiasLog)				/** log(x) */
DO1(neg,BiasNeg)				/** neg(x) */
DO1(sin,BiasSin)				/** sin(x) */
DO1(sinh,BiasSinh)				/** sinh(x) */
DO1(sqr,BiasSqr)				/** sqr(x) */
DO1(sqrt,BiasSqrt)				/** sqrt(x) */
DO1(tan,BiasTan)				/** tan(x) */
DO1(tanh,BiasTanh)				/** tanh(x) */
DO2(add,BiasAddII)				/** add(x,y) */
DO2(div,BiasDivII)				/** div(x,y) */
DO2(join,BiasHullII)				/** join(x,y) */
DO2(mul,BiasMulII)				/** mul(x,y) */
DO2(pow,BiasPowerI)				/** pow(x,y) */
DO2(sub,BiasSubII)				/** sub(x,y) */

static const luaL_Reg R[] =
{
	{ "__add",	Ladd	},		/** __add(x,y) */
	{ "__div",	Ldiv	},		/** __div(x,y) */
	{ "__le",	Lle	},		/** __le(x,y) */
	{ "__lt",	Llt	},		/** __lt(x,y) */
	{ "__mul",	Lmul	},		/** __mul(x,y) */
	{ "__pow",	Lpow	},		/** __pow(x,y) */
	{ "__sub",	Lsub	},		/** __sub(x,y) */
	{ "__tostring",	Ltostring},		/** __tostring(x) */
	{ "__unm",	Lneg	},		/** __unm(x) */
#define DECLARE(f)	{ #f, L##f },
DECLARE(abs)
DECLARE(acos)
DECLARE(acosh)
DECLARE(acot)
DECLARE(acoth)
DECLARE(add)
DECLARE(asin)
DECLARE(asinh)
DECLARE(atan)
DECLARE(atanh)
DECLARE(contains)
DECLARE(contained)
DECLARE(cos)
DECLARE(cosh)
DECLARE(cot)
DECLARE(coth)
DECLARE(diam)
DECLARE(div)
DECLARE(exp)
DECLARE(extremes)
DECLARE(inf)
DECLARE(ipow)
DECLARE(join)
DECLARE(log)
DECLARE(log10)
DECLARE(meet)
DECLARE(mid)
DECLARE(midrad)
DECLARE(mul)
DECLARE(neg)
DECLARE(new)
DECLARE(pow)
DECLARE(root)
DECLARE(sin)
DECLARE(sinh)
DECLARE(sqr)
DECLARE(sqrt)
DECLARE(sub)
DECLARE(sup)
DECLARE(tan)
DECLARE(tanh)
DECLARE(tostring)
DECLARE(type)
	{ NULL,		NULL	}
};

LUALIB_API int luaopen_interval(lua_State *L)
{
 BiasFuncInit();				/* calls BiasInit */
 luaL_newmetatable(L,MYTYPE);
 lua_setglobal(L,MYNAME);
 luaL_register(L,MYNAME,R);
 lua_pushliteral(L,"version");			/** version */
 lua_pushliteral(L,MYVERSION);
 lua_settable(L,-3);
 lua_pushliteral(L,"__index");
 lua_pushvalue(L,-2);
 lua_settable(L,-3);
 return 1;
}
