ASP上传无限多文件版

大部分ASP上传源码只支持单个文件上传或几个，我需要一个接着一个上传...上传无限多个。
所以按自己需求编写了此代码。此代码在无惧上传类 V1.2基础上修改，实现无限多图片上传，支持显示上传后的文件名，实时把图片插入编辑框。

test.asp 为测试文件。
修改这段：onclick="test('<img src=' + document.form1.pic.value + '></img>')"  融合你的编辑器格式。
举例：
onclick="test('<img>' + document.form1.pic.value + '</img>')"
onclick="test('[IMG]' + document.form1.pic.value + '[/IMG]')"

作者：guke
QQ：6692103