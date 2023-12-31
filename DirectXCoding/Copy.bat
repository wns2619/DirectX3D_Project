// 명령어		옵션		원본파일 위치					사본파일 저장 위치
	
xcopy		/y		.\Engine\Public\*.*						.\Reference\Headers\		
xcopy		/y		.\Engine\Binaries\*.dll					.\DirectXCoding\Binaries\
xcopy		/y		.\Engine\Binaries\*.lib					.\Reference\Libraries\Debug\

xcopy		/y		.\Engine\BinariesR\*.lib				.\Reference\Libraries\Release\
xcopy		/y		.\Engine\BinariesR\*.dll				.\DirectXCoding\BinariesR\

xcopy		/y		.\Engine\Binaries\Shaders\*.*					.\DirectXCoding\Binaries\Shaders\

xcopy		/y		.\Engine\BinariesR\*.dll						.\DirectXCoding\BinariesR\
xcopy		/y		.\Engine\BinariesR\Shaders\*.*					.\DirectXCoding\BinariesR\Shaders\