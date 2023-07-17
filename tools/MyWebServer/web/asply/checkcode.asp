<%Option Explicit
NumCode
Function NumCode()

	Response.Expires = -1
 ' msgbox 1
	Response.AddHeader "Pragma", "no-cache"
 ' msgbox 2
	Response.AddHeader "cache-control", "no-cache"
	'On Error Resume Next
'response.end 
	Dim zNum,i,j
	Dim Ados,Ados1
	Randomize timer
	zNum = cint(8999*Rnd+1000)
'response.end 
	Session("CheckCode") =  zNum
 'msgbox Session("CheckCode")

	Dim zimg(4),NStr

	NStr=cstr(zNum)
 'response.end 
	For i=0 To 3
		zimg(i)=cint(mid(NStr,i+1,1))
	Next
	Dim Pos
	Set Ados=Server.CreateObject("Adodb.Stream")
	Ados.Mode=3
	Ados.Type=1
	Ados.Open
	Set Ados1=Server.CreateObject("Adodb.Stream")
	Ados1.Mode=3
	Ados1.Type=1
	Ados1.Open
	Ados.LoadFromFile(Server.mappath("images/body.Fix" ))
'msgbox Server.mappath("images/body.Fix" )
	Ados1.write Ados.read(1280)
 
	For i=0 To 3
		Ados.Position=(9-zimg(i))*320
		Ados1.Position=i*320
             '  msgbox 33
		Ados1.write ados.read(320)
              ' msgbox 44
	Next	
   '            msgbox 55
	Ados.LoadFromFile(Server.mappath("images/head.fix" ))
    '           msgbox 57
	Pos=lenb(Ados.read())
           
	Ados.Position=Pos
'msgbox 66
	For i=0 To 9 Step 1
		For j=0 To 3
			Ados1.Position=i*32+j*320
			Ados.Position=Pos+30*j+i*120
			Ados.write ados1.read(30)
		Next
	Next

	Response.ContentType = "image/BMP"
	Ados.Position=0
   'msgbox  lenb(Ados.read())
	Response.BinaryWrite Ados.read() 
	Ados.Close:set Ados=nothing
	Ados1.Close:set Ados1=nothing
 'msgbox Session("CheckCode")
	If Err Then Session("CheckCode") = 9999
End Function
%>