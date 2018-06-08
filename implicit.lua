-- test interval library: plot implicit curve f(x,y)=0

require"interval"
interval.__pow=interval.ipow

function f(x,y)
	return y^2-x^3+x
end

function explore(x,y)
	local ax,bx=x:extremes()
	local ay,by=y:extremes()
	box(ax,bx,ay,by,"b")
	if f(x,y):contains(0) then
		if x:diam()<2^(-6) then
			box(ax,bx,ay,by,"B")
		else
			local xlo,xhi=split(x)
			local ylo,yhi=split(y)
			explore(xlo,ylo)
			explore(xhi,ylo)
			explore(xlo,yhi)
			explore(xhi,yhi)
		end
	end
end

function split(x)
	local ax,bx=x:extremes()
	local mx=x:mid()
	local xlo=interval.new(ax,mx)
	local xhi=interval.new(mx,bx)
	return xlo,xhi
end

function output(...)
	for i=1,arg.n do
		io.write(arg[i]," ")
	end
	io.write("\n")
end

function box(xmin,xmax,ymin,ymax,t)
	local dx=xmax-xmin
	local dy=ymax-ymin
	output(xmin,ymin,dx,dy,t)
end

function plot(xmin,xmax,ymin,ymax)
	local X0,Y0,DX,DY=50,50,500,500
	output("%!PS-Adobe-2.0 EPSF-2.0")
	output("%%BoundingBox:",X0,Y0,X0+DX,Y0+DY)
	output("%%Title: implicit curve")
	output("%%Creator:",interval.version)
	output("%%CreationDate:",os.date())
	output("%%Pages: 1")
	output("%%EndComments")
	output("%%EndProlog")
	output("%%BeginSetup")
	output("/b { 0 setgray rectstroke } bind def")
	output("/B { 4 copy 0.8 setgray rectfill b } bind def")
	output("0 setlinewidth")
	output(X0,Y0,"translate")
	output(DX,DY,"scale")
	output("%%EndSetup\n")
	output("%%Page: 1 1")
	local dx=xmax-xmin
	local dy=ymax-ymin
	output("% region is",xmin,xmax,ymin,ymax)
	output("1",dx,"div","1",dy,"div scale")
	output(-xmin,-ymin,"translate\n")
	explore(interval.new(xmin,xmax),interval.new(ymin,ymax))
	output("\nshowpage")
	output("%%EOF")
end

plot(-2,3,-3,2)
