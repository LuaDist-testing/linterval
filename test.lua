-- test interval library

require"interval"
interval.__pow=interval.ipow

------------------------------------------------------------------------------
print(interval.version)

function new(a,b)
	local x=interval.new(a,b)
	print""
	print(x)
	print("new",x:extremes())
	return x
end

function test(w,x,y)
	print(w,interval[w](x,y):extremes())
end

x=new(-1,2)
test("neg",x)
test("add",x,x)
test("sub",x,x)
test("mul",x,x)
test("div",x,2)
test("sqr",x)
test("sqrt",x:sqr())
test("abs",x)

x=new(4,9)
print("midrad",x:midrad())
test("sqrt",x)
test("root",x,2)
test("root",x,3)
print("old",x:extremes())
test("log",x)

x=new(-1,2)
test("sqr",x)
test("ipow",x,2)
test("ipow",x,3)

interval.id= function (x) return x end

x=new(-1,2)
test("id",x^2)
test("id",x^3)

y=new(0,1)
test("id",y)
test("join",x,y)
test("meet",x,y)
print(interval.contained(y,x))

Z=interval.new(0)

function test(a,b)
	X=interval.new(a,b)
	print(a,b,X<Z,X<=Z,X>Z,X>=Z)
end

print""
print("","X","X<0","X<=0","X>0","X>=0")
test(-3,-2)
test(-3,0)
test(-3,2)
test(0,2)
test(1,2)

------------------------------------------------------------------------------
print""
print(interval.version)
