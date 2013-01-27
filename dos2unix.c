//debug =============================
#ifndef MYDEBUG
	#define MYDEBUG 0
#endif

#ifndef DEBUG_dos2unix_c
	#define DEBUG_dos2unix_c 1
//	#define DEBUG_dos2unix_c 0
#endif

#ifndef DEBUG_special
	#define DEBUG_special 0
#endif
//////////////////////////////////////debug

	PrintBSDLicense();
	wint_t TempNextChar;
	/* CurChar is always CR (x0d) */
	/* In normal dos2unix mode put nothing (skip CR). */
	/* Don't modify Mac files when in dos2unix mode. */
	if ( (TempNextChar = d2u_getwc(ipInF, ipFlag->bomtype)) != WEOF) {
		d2u_ungetwc( TempNextChar, ipInF, ipFlag->bomtype);  /* put back peek char */
		if ( TempNextChar != 0x0a ) {
			d2u_putwc( CurChar, ipOutF, ipFlag);  /* Mac line, put back CR */
		}
	}
	else if ( CurChar == 0x0d ) {  /* EOF: last Mac line delimiter (CR)? */
		d2u_putwc( CurChar, ipOutF, ipFlag);
	}
	if (ipFlag->NewLine) {  /* add additional LF? */
		d2u_putwc(0x0a, ipOutF, ipFlag);
	}
	int TempNextChar;
	/* CurChar is always CR (x0d) */
	/* In normal dos2unix mode put nothing (skip CR). */
	/* Don't modify Mac files when in dos2unix mode. */
	if ( (TempNextChar = fgetc(ipInF)) != EOF) {
		ungetc( TempNextChar, ipInF );  /* put back peek char */
		if ( TempNextChar != '\x0a' ) {
			fputc( CurChar, ipOutF );  /* Mac line, put back CR */
		}
	}
	else if ( CurChar == '\x0d' ) {  /* EOF: last Mac line delimiter (CR)? */
		fputc( CurChar, ipOutF );
	}
	if (ipFlag->NewLine) {  /* add additional LF? */
		fputc('\x0a', ipOutF);
	}
		int RetVal = 0;
		wint_t TempChar;
		wint_t TempNextChar;
		int line_nr = 1;

		ipFlag->status = 0;

		/* CR-LF -> LF */
		/* LF    -> LF, in case the input file is a Unix text file */
		/* CR    -> CR, in dos2unix mode (don't modify Mac file) */
		/* CR    -> LF, in Mac mode */
		/* \x0a = Newline/Line Feed (LF) */
		/* \x0d = Carriage Return (CR) */

		switch (ipFlag->FromToMode)
		{
			case FROMTO_DOS2UNIX: /* dos2unix */
				while ((TempChar = d2u_getwc(ipInF, ipFlag->bomtype)) != WEOF) {  /* get character */
				  if ((ipFlag->Force == 0) &&
				      (TempChar < 32) &&
				      (TempChar != 0x0a) &&  /* Not an LF */
				      (TempChar != 0x0d) &&  /* Not a CR */
				      (TempChar != 0x09) &&  /* Not a TAB */
				      (TempChar != 0x0c)) {  /* Not a form feed */
				    RetVal = -1;
				    ipFlag->status |= BINARY_FILE ;
				    if (!ipFlag->Quiet)
				    {
				      fprintf(stderr, "%s: ", progname);
				      fprintf(stderr, _("Binary symbol found at line %d\n"), line_nr);
				    }
				    break;
				  }
				  if (TempChar != 0x0d) {
				    if (TempChar == 0x0a) /* Count all DOS and Unix line breaks */
				      ++line_nr;
				    if (d2u_putwc(TempChar, ipOutF, ipFlag) == WEOF) {
				      RetVal = -1;
				      if (!ipFlag->Quiet)
				      {
				        if (!(ipFlag->status & UNICODE_CONVERSION_ERROR))
				        {
				          fprintf(stderr, "%s: ", progname);
				          fprintf(stderr, "%s", _("can not write to output file\n"));
				        }
				      }
				      break;
				    }
				  } else {
				    StripDelimiterW( ipInF, ipOutF, ipFlag, TempChar );
				  }
				}
				break;
			case FROMTO_MAC2UNIX: /* mac2unix */
				while ((TempChar = d2u_getwc(ipInF, ipFlag->bomtype)) != WEOF) {
				  if ((ipFlag->Force == 0) &&
				      (TempChar < 32) &&
				      (TempChar != 0x0a) &&  /* Not an LF */
				      (TempChar != 0x0d) &&  /* Not a CR */
				      (TempChar != 0x09) &&  /* Not a TAB */
				      (TempChar != 0x0c)) {  /* Not a form feed */
				    RetVal = -1;
				    ipFlag->status |= BINARY_FILE ;
				    if (!ipFlag->Quiet)
				    {
				      fprintf(stderr, "%s: ", progname);
				      fprintf(stderr, _("Binary symbol found at line %d\n"), line_nr);
				    }
				    break;
				  }
				  if ((TempChar != 0x0d))
				    {
				      if (TempChar == 0x0a) /* Count all DOS and Unix line breaks */
				        ++line_nr;
				      if(d2u_putwc(TempChar, ipOutF, ipFlag) == WEOF){
				        RetVal = -1;
				        if (!ipFlag->Quiet)
				        {
				          if (!(ipFlag->status & UNICODE_CONVERSION_ERROR))
				          {
				            fprintf(stderr, "%s: ", progname);
				            fprintf(stderr, "%s", _("can not write to output file\n"));
				          }
				        }
				        break;
				      }
				    }
				  else{
				    /* TempChar is a CR */
				    if ( (TempNextChar = d2u_getwc(ipInF, ipFlag->bomtype)) != WEOF) {
				      ungetc( TempNextChar, ipInF );  /* put back peek char */
				      /* Don't touch this delimiter if it's a CR,LF pair. */
				      if ( TempNextChar == 0x0a ) {
				        d2u_putwc(0x0d, ipOutF, ipFlag); /* put CR, part of DOS CR-LF */
				        continue;
				      }
				    }
				    if (d2u_putwc(0x0a, ipOutF, ipFlag) == WEOF) /* MAC line end (CR). Put LF */
				      {
				        RetVal = -1;
				        if (!ipFlag->Quiet)
				        {
				          if (!(ipFlag->status & UNICODE_CONVERSION_ERROR))
				          {
				            fprintf(stderr, "%s: ", progname);
				            fprintf(stderr, "%s", _("can not write to output file\n"));
				          }
				        }
				        break;
				      }
				    line_nr++; /* Count all Mac line breaks */
				    if (ipFlag->NewLine) {  /* add additional LF? */
				      d2u_putwc(0x0a, ipOutF, ipFlag);
				    }
				  }
				}
				break;
			default: /* unknown FromToMode */
			;
			fprintf(stderr, "%s: ", progname);
			fprintf(stderr, _("program error, invalid conversion mode %d\n"),ipFlag->FromToMode);
			exit(1);
		}
		return RetVal;

		int RetVal = 0;
		int TempChar;
		int TempNextChar;
		int *ConvTable;
		int line_nr = 1;

		ipFlag->status = 0;

		switch (ipFlag->ConvMode)
		{
			case CONVMODE_ASCII: /* ascii */
				ConvTable = D2UAsciiTable;
				break;
			case CONVMODE_7BIT: /* 7bit */
				ConvTable = D2U7BitTable;
				break;
			case CONVMODE_437: /* iso */
				ConvTable = D2UIso437Table;
				break;
			case CONVMODE_850: /* iso */
				ConvTable = D2UIso850Table;
				break;
			case CONVMODE_860: /* iso */
				ConvTable = D2UIso860Table;
				break;
			case CONVMODE_863: /* iso */
				ConvTable = D2UIso863Table;
				break;
			case CONVMODE_865: /* iso */
				ConvTable = D2UIso865Table;
				break;
			case CONVMODE_1252: /* iso */
				ConvTable = D2UIso1252Table;
				break;
			default: /* unknown convmode */
				ipFlag->status |= WRONG_CODEPAGE ;
				return(-1);
		}
		if ((ipFlag->ConvMode > 1) && (!ipFlag->Quiet)) /* not ascii or 7bit */
		{
			 fprintf(stderr, "%s: ", progname);
			 fprintf(stderr, _("using code page %d.\n"), ipFlag->ConvMode);
		}

		/* CR-LF -> LF */
		/* LF    -> LF, in case the input file is a Unix text file */
		/* CR    -> CR, in dos2unix mode (don't modify Mac file) */
		/* CR    -> LF, in Mac mode */
		/* \x0a = Newline/Line Feed (LF) */
		/* \x0d = Carriage Return (CR) */

		switch (ipFlag->FromToMode)
		{
			case FROMTO_DOS2UNIX: /* dos2unix */
				while ((TempChar = fgetc(ipInF)) != EOF) {  /* get character */
				  if ((ipFlag->Force == 0) &&
				      (TempChar < 32) &&
				      (TempChar != '\x0a') &&  /* Not an LF */
				      (TempChar != '\x0d') &&  /* Not a CR */
				      (TempChar != '\x09') &&  /* Not a TAB */
				      (TempChar != '\x0c')) {  /* Not a form feed */
				    RetVal = -1;
				    ipFlag->status |= BINARY_FILE ;
				    if (!ipFlag->Quiet)
				    {
				      fprintf(stderr, "%s: ", progname);
				      fprintf(stderr, _("Binary symbol found at line %d\n"), line_nr);
				    }
				    break;
				  }
				  if (TempChar != '\x0d') {
				    if (TempChar == '\x0a') /* Count all DOS and Unix line breaks */
				      ++line_nr;
				    if (fputc(ConvTable[TempChar], ipOutF) == EOF) {
				      RetVal = -1;
				      if (!ipFlag->Quiet)
				      {
				        fprintf(stderr, "%s: ", progname);
				        fprintf(stderr, "%s", _("can not write to output file\n"));
				      }
				      break;
				    }
				  } else {
				    StripDelimiter( ipInF, ipOutF, ipFlag, TempChar );
				  }
				}
				break;
			case FROMTO_MAC2UNIX: /* mac2unix */
				while ((TempChar = fgetc(ipInF)) != EOF) {
				  if ((ipFlag->Force == 0) &&
				      (TempChar < 32) &&
				      (TempChar != '\x0a') &&  /* Not an LF */
				      (TempChar != '\x0d') &&  /* Not a CR */
				      (TempChar != '\x09') &&  /* Not a TAB */
				      (TempChar != '\x0c')) {  /* Not a form feed */
				    RetVal = -1;
				    ipFlag->status |= BINARY_FILE ;
				    if (!ipFlag->Quiet)
				    {
				      fprintf(stderr, "%s: ", progname);
				      fprintf(stderr, _("Binary symbol found at line %d\n"), line_nr);
				    }
				    break;
				  }
				  if ((TempChar != '\x0d'))
				    {
				      if (TempChar == '\x0a') /* Count all DOS and Unix line breaks */
				        ++line_nr;
				      if(fputc(ConvTable[TempChar], ipOutF) == EOF){
				        RetVal = -1;
				        if (!ipFlag->Quiet)
				        {
				          fprintf(stderr, "%s: ", progname);
				          fprintf(stderr, "%s", _("can not write to output file\n"));
				        }
				        break;
				      }
				    }
				  else{
				    /* TempChar is a CR */
				    if ( (TempNextChar = fgetc(ipInF)) != EOF) {
				      ungetc( TempNextChar, ipInF );  /* put back peek char */
				      /* Don't touch this delimiter if it's a CR,LF pair. */
				      if ( TempNextChar == '\x0a' ) {
				        fputc('\x0d', ipOutF); /* put CR, part of DOS CR-LF */
				        continue;
				      }
				    }
				    if (fputc('\x0a', ipOutF) == EOF) /* MAC line end (CR). Put LF */
				      {
				        RetVal = -1;
				        if (!ipFlag->Quiet)
				        {
				          fprintf(stderr, "%s: ", progname);
				          fprintf(stderr, "%s", _("can not write to output file\n"));
				        }
				        break;
				      }
				    line_nr++; /* Count all Mac line breaks */
				    if (ipFlag->NewLine) {  /* add additional LF? */
				      fputc('\x0a', ipOutF);
				    }
				  }
				}
				break;
			default: /* unknown FromToMode */
			;
			fprintf(stderr, "%s: ", progname);
			fprintf(stderr, _("program error, invalid conversion mode %d\n"),ipFlag->FromToMode);
			exit(1);
		}
		return RetVal;

	int RetVal = 0;
	FILE *InF = NULL;
	FILE *TempF = NULL;
	char *TempPath;
	char *errstr;
	struct stat StatBuf;
	struct utimbuf UTimeBuf;
	mode_t mask;
	FILE* fd;
	int fd;
	char *TargetFN = NULL;
	int ResolveSymlinkResult = 0;

	ipFlag->status = 0 ;

	/* Test if output file is a symbolic link */
	if (symbolic_link(ipOutFN) && !ipFlag->Follow)
	{
		ipFlag->status |= OUTPUTFILE_SYMLINK ;
		/* Not a failure, skipping input file according spec. (keep symbolic link unchanged) */
		return -1;
	}

	/* Test if input file is a regular file or symbolic link */
	if (regfile(ipInFN, 1, ipFlag, progname))
	{
		ipFlag->status |= NO_REGFILE ;
		/* Not a failure, skipping non-regular input file according spec. */
		return -1;
	}

	/* Test if input file target is a regular file */
	if (symbolic_link(ipInFN) && regfile_target(ipInFN, ipFlag,progname))
	{
		ipFlag->status |= INPUT_TARGET_NO_REGFILE ;
		/* Not a failure, skipping non-regular input file according spec. */
		return -1;
	}

	/* Test if output file target is a regular file */
	if (symbolic_link(ipOutFN) && (ipFlag->Follow == SYMLINK_FOLLOW) && regfile_target(ipOutFN, ipFlag,progname))
	{
		ipFlag->status |= OUTPUT_TARGET_NO_REGFILE ;
		/* Failure, input is regular, cannot produce output. */
		if (!ipFlag->error) ipFlag->error = 1;
		return -1;
	}

	/* retrieve ipInFN file date stamp */
	if (stat(ipInFN, &StatBuf))
	{
		if (!ipFlag->Quiet)
		{
			ipFlag->error = errno;
			errstr = strerror(errno);
			fprintf(stderr, "%s: %s: %s\n", progname, ipInFN, errstr);
		}
		RetVal = -1;
	}
	if((fd = MakeTempFileFrom(ipOutFN, &TempPath))==NULL) {
	if((fd = MakeTempFileFrom (ipOutFN, &TempPath)) < 0) {
		if (!ipFlag->Quiet)
		{
			ipFlag->error = errno;
			errstr = strerror(errno);
			fprintf(stderr, "%s: ", progname);
			fprintf(stderr, _("Failed to open temporary output file: %s\n"), errstr);
		}
		RetVal = -1;
	}
	fprintf(stderr, "%s: ", progname);
	fprintf(stderr, _("using %s as temporary file\n"), TempPath);
	/* can open in file? */
	if (!RetVal)
	{
		InF=OpenInFile(ipInFN);
		if (InF == NULL)
		{
			ipFlag->error = errno;
			errstr = strerror(errno);
			fprintf(stderr, "%s: %s: %s\n", progname, ipInFN, errstr);
			RetVal = -1;
		}
	}

	/* can open output file? */
	if ((!RetVal) && (InF))
	{
		if ((TempF=fd) == NULL)
		{
		if ((TempF=OpenOutFile(fd)) == NULL)
		{
			ipFlag->error = errno;
			errstr = strerror(errno);
			fprintf(stderr, "%s: %s\n", progname, errstr);
			fclose (InF);
			InF = NULL;
			RetVal = -1;
		}
	}
	InF = read_bom(InF, &ipFlag->bomtype);
	if ((ipFlag->bomtype == FILE_UTF16LE) || (ipFlag->bomtype == FILE_UTF16BE))
	{
		if (strcmp(nl_langinfo(CODESET), "UTF-8") != 0)
		{
			/* Don't convert UTF-16 files when the locale encoding is not UTF-8
			 * to prevent loss of characters. */
			ipFlag->status |= LOCALE_NOT_UTF8 ;
			if (!ipFlag->error) ipFlag->error = 1;
			RetVal = -1;
		}
	}
	if ((ipFlag->bomtype == FILE_UTF16LE) || (ipFlag->bomtype == FILE_UTF16BE))
	{
		if (sizeof(wchar_t) < 4)
		{
			/* A decoded UTF-16 surrogate pair must fit in a wchar_t */
			ipFlag->status |= WCHAR_T_TOO_SMALL ;
			if (!ipFlag->error) ipFlag->error = 1;
			RetVal = -1;
		}
	}
	if (ipFlag->add_bom)
		fprintf(TempF, "%s", "\xEF\xBB\xBF");  /* UTF-8 BOM */
	/* Turn off ISO and 7-bit conversion for Unicode text files */
	if (ipFlag->bomtype > 0)
		ipFlag->ConvMode = CONVMODE_ASCII;
	/* conversion sucessful? */
	if ((ipFlag->bomtype == FILE_UTF16LE) || (ipFlag->bomtype == FILE_UTF16BE))
	{
		if ((!RetVal) && (ConvertDosToUnixW(InF, TempF, ipFlag, progname)))
			RetVal = -1;
		if (ipFlag->status & UNICODE_CONVERSION_ERROR)
		{
			if (!ipFlag->error) ipFlag->error = 1;
			RetVal = -1;
		}
	} else {
		if ((!RetVal) && (ConvertDosToUnix(InF, TempF, ipFlag, progname)))
			RetVal = -1;
	}
	if ((!RetVal) && (ConvertDosToUnix(InF, TempF, ipFlag, progname)))
		RetVal = -1;
	 /* can close in file? */
	if ((InF) && (fclose(InF) == EOF))
		RetVal = -1;
	/* can close output file? */
	if ((TempF) && (fclose(TempF) == EOF))
		RetVal = -1;
	if(fd!=NULL)
		fclose(fd);
	if(fd>=0)
		close(fd);
	if (!RetVal)
	{
		if (ipFlag->NewFile == 0) /* old file mode */
		{
			 RetVal = chmod (TempPath, StatBuf.st_mode); /* set original permissions */
		}
		else
		{
			 mask = umask(0); /* get process's umask */
			 umask(mask); /* set umask back to original */
			 RetVal = chmod(TempPath, StatBuf.st_mode & ~mask); /* set original permissions, minus umask */
		}

		if (RetVal)
		{
			 if (!ipFlag->Quiet)
			 {
				 ipFlag->error = errno;
				 errstr = strerror(errno);
				 fprintf(stderr, "%s: ", progname);
				 fprintf(stderr, _("Failed to change the permissions of temporary output file %s: %s\n"), TempPath, errstr);
			 }
		}
	}
	if (!RetVal && (ipFlag->NewFile == 0))  /* old file mode */
	{
		 /* Change owner and group of the the tempory output file to the original file's uid and gid. */
		 /* Required when a different user (e.g. root) has write permission on the original file. */
		 /* Make sure that the original owner can still access the file. */
		 if (chown(TempPath, StatBuf.st_uid, StatBuf.st_gid))
		 {
				if (!ipFlag->Quiet)
				{
				  ipFlag->error = errno;
				  errstr = strerror(errno);
				  fprintf(stderr, "%s: ", progname);
				  fprintf(stderr, _("Failed to change the owner and group of temporary output file %s: %s\n"), TempPath, errstr);
				}
				RetVal = -1;
		 }
	}
	if ((!RetVal) && (ipFlag->KeepDate))
	{
		UTimeBuf.actime = StatBuf.st_atime;
		UTimeBuf.modtime = StatBuf.st_mtime;
		/* can change output file time to in file time? */
		if (utime(TempPath, &UTimeBuf) == -1)
		{
			if (!ipFlag->Quiet)
			{
				ipFlag->error = errno;
				errstr = strerror(errno);
				fprintf(stderr, "%s: %s: %s\n", progname, TempPath, errstr);
			}
			RetVal = -1;
		}
	}

	/* any error? cleanup the temp file */
	if (RetVal && (TempPath != NULL))
	{
		if (unlink(TempPath) && (errno != ENOENT))
		{
			if (!ipFlag->Quiet)
			{
				ipFlag->error = errno;
				errstr = strerror(errno);
				fprintf(stderr, "%s: %s: %s\n", progname, TempPath, errstr);
			}
			RetVal = -1;
		}
	}

	/* If output file is a symbolic link, optional resolve the link and modify  */
	/* the target, instead of removing the link and creating a new regular file */
	TargetFN = ipOutFN;
	if (symbolic_link(ipOutFN) && !RetVal)
	{
		ResolveSymlinkResult = 0; /* indicates that TargetFN need not be freed */
		if (ipFlag->Follow == SYMLINK_FOLLOW)
		{
			ResolveSymlinkResult = ResolveSymbolicLink(ipOutFN, &TargetFN, ipFlag, progname);
			if (ResolveSymlinkResult < 0)
			{
				if (!ipFlag->Quiet)
				{
				  fprintf(stderr, "%s: ", progname);
				  fprintf(stderr, _("problems resolving symbolic link '%s'\n"), ipOutFN);
				  fprintf(stderr, _("          output file remains in '%s'\n"), TempPath);
				}
				RetVal = -1;
			}
		}
	}

	/* can rename temporary file to output file? */
	if (!RetVal)
	{
		if (unlink(TargetFN) && (errno != ENOENT))
		{
			if (!ipFlag->Quiet)
			{
				ipFlag->error = errno;
				errstr = strerror(errno);
				fprintf(stderr, "%s: %s: %s\n", progname, TargetFN, errstr);
			}
			RetVal = -1;
		}
		if (rename(TempPath, TargetFN) == -1)
		{
			if (!ipFlag->Quiet)
			{
				ipFlag->error = errno;
				errstr = strerror(errno);
				fprintf(stderr, "%s: ", progname);
				fprintf(stderr, _("problems renaming '%s' to '%s': %s\n"), TempPath, TargetFN, errstr);
				if (ResolveSymlinkResult > 0)
				  fprintf(stderr, _("          which is the target of symbolic link '%s'\n"), ipOutFN);
				fprintf(stderr, _("          output file remains in '%s'\n"), TempPath);
			}
			RetVal = -1;
		}

		if (ResolveSymlinkResult > 0)
			free(TargetFN);
	}
	free(TempPath);
	return RetVal;


		ipFlag->NewFile = 1;
		ipFlag->Quiet = 1;
		ipFlag->KeepDate = 0;
		ipFlag->Force = 1;
		/* stdin and stdout are by default text streams. We need
		 * to set them to binary mode. Otherwise an LF will
		 * automatically be converted to CR-LF on DOS/Windows.
		 * Erwin */
		/* 'setmode' was deprecated by MicroSoft
		 * since Visual C++ 2005. Use '_setmode' instead. */
		_setmode(fileno(stdout), O_BINARY);
		_setmode(fileno(stdin), O_BINARY);
		setmode(fileno(stdout), O_BINARY);
		setmode(fileno(stdin), O_BINARY);
		read_bom(stdin, &ipFlag->bomtype);
		if (ipFlag->add_bom)
			 fprintf(stdout, "%s", "\xEF\xBB\xBF");  /* UTF-8 BOM */
		if ((ipFlag->bomtype == FILE_UTF16LE) || (ipFlag->bomtype == FILE_UTF16BE))
		{
			 return (ConvertDosToUnixW(stdin, stdout, ipFlag, progname));
		} else {
			 return (ConvertDosToUnix(stdin, stdout, ipFlag, progname));
		}
		return (ConvertDosToUnix(stdin, stdout, ipFlag, progname));


//debug =============================
#if MYDEBUG | DEBUG_dos2unix_c
	printf("[%s:%d]: %s\n", __FILE__, __LINE__, "========== Enter: main() ========");

#endif
//////////////////////////////////////debug



	/* variable declarations */
	char progname[9];
	int ArgIdx;
	int CanSwitchFileMode;
	int ShouldExit;
	int RetVal = 0;
	int process_options = 1;
	CFlag *pFlag;
	char *ptr;

//debug =============================
#if MYDEBUG | DEBUG_dos2unix_c
	printf("[%s:%d]: %s\n", __FILE__, __LINE__, "ENABLE_NLS => Defined");

#endif
//////////////////////////////////////debug

	char localedir[1024];
//debug =============================
#if MYDEBUG | DEBUG_dos2unix_c
#else
	printf("[%s:%d]: %s\n", __FILE__, __LINE__, "ENABLE_NLS => Not defined");
#endif
//////////////////////////////////////debug


	int _dowildcard = -1; /* enable wildcard expansion for Win64 */
	progname[8] = '\0';
	strcpy(progname,"dos2unix");
//debug =============================
#if MYDEBUG | DEBUG_dos2unix_c

	printf("[%s:%d]: progname=%s\n", __FILE__, __LINE__, progname);


#endif//#if MYDEBUG | DEBUG_dos2unix_c
//////////////////////////////////////debug

	 ptr = getenv("DOS2UNIX_LOCALEDIR");

//debug =============================
#if MYDEBUG | DEBUG_dos2unix_c
	printf("[%s:%d]: ", __FILE__, __LINE__);

	if (ptr == NULL) {

		printf("%s\n", "ptr == NULL");

	} else {//if (ptr == NULL)

		printf("%s: ptr=%s\n", "ptr != NULL", ptr);

	}//if (ptr == NULL)


#endif//#if MYDEBUG | DEBUG_dos2unix_c
//////////////////////////////////////debug

	 if (ptr == NULL)

//debug ===============================================
#if MYDEBUG | DEBUG_dos2unix_c
	 {
		 printf("[%s:%d]: ", __FILE__, __LINE__);
//		 printf("LOCALEDIR=%s (%d)\n", LOCALEDIR, strlen(ptr));	//=> Segmentation fault (コアダンプ)
		 printf("LOCALEDIR=%s\n", LOCALEDIR);


#endif//#if MYDEBUG | DEBUG_dos2unix.c
//////////////////////////////////////////////////debug

			strcpy(localedir,LOCALEDIR);

//debug ===============================================
#if MYDEBUG | DEBUG_dos2unix_c
			printf("[%s:%d]: ", __FILE__, __LINE__);
	 }

#endif//#if MYDEBUG | DEBUG_dos2unix.c
//////////////////////////////////////////////////debug

	 else
	 {

//debug =============================
#if MYDEBUG | DEBUG_dos2unix_c
	printf("[%s:%d]: ", __FILE__, __LINE__);
	printf("[%s:%d]: ptr=%d/localedir=%d\n",
							__FILE__, __LINE__, strlen(ptr), strlen(localedir));
#endif//#if MYDEBUG | DEBUG_dos2unix_c
//////////////////////////////////////debug

			if (strlen(ptr) < sizeof(localedir))

//debug =============================
#if MYDEBUG | DEBUG_dos2unix_c
			{
				printf("[%s:%d]: %s\n", __FILE__, __LINE__, "strlen(ptr) < sizeof(localedir)");
				printf("[%s:%d]: ptr=%d/localedir=%d\n",
						__FILE__, __LINE__, strlen(ptr), strlen(localedir));

#endif//#if MYDEBUG | DEBUG_dos2unix_c
//////////////////////////////////////debug

				 strcpy(localedir,ptr);

//debug =============================
#if MYDEBUG | DEBUG_dos2unix_c
			}
#endif//#if MYDEBUG | DEBUG_dos2unix.c
//////////////////////////////////////debug

			else
			{
				 fprintf(stderr,"%s: ",progname);
				 fprintf(stderr, "%s", _("error: Value of environment variable DOS2UNIX_LOCALEDIR is too long.\n"));
				 strcpy(localedir,LOCALEDIR);
			}
	 }


//	 setlocale (LC_ALL, "");
//debug ===============================================
#if MYDEBUG | DEBUG_dos2unix_c
	printf("[%s:%d]: ", __FILE__, __LINE__);

	char* loc = setlocale (LC_ALL, "");

	float x = 0.2376;

	printf("locale set => %s\n", loc);

#if DEBUG_special
	printf("x => %f\n", x);

	loc = setlocale (LC_ALL, "FRA");

	printf("locale set => %s\n", loc);
	printf("x => %f\n", x);
#endif//#if DEBUG_special

#endif//#if MYDEBUG | DEBUG_dos2unix_c
//////////////////////////////////////////////////debug

	 bindtextdomain (PACKAGE, localedir);
	 textdomain (PACKAGE);
	/* variable initialisations */
	ArgIdx = 0;
	CanSwitchFileMode = 1;
	ShouldExit = 0;
	pFlag = (CFlag*)malloc(sizeof(CFlag));
	pFlag->NewFile = 0;
	pFlag->Quiet = 0;
	pFlag->KeepDate = 0;
	pFlag->ConvMode = CONVMODE_ASCII;  /* default ascii */
	pFlag->FromToMode = FROMTO_DOS2UNIX;  /* default dos2unix */
	pFlag->NewLine = 0;
	pFlag->Force = 0;
	pFlag->Follow = SYMLINK_SKIP;
	pFlag->status = 0;
	pFlag->stdio_mode = 1;
	pFlag->error = 0;

	pFlag->bomtype = FILE_MBS;

	//debug ===============================================
	#if MYDEBUG | DEBUG_dos2unix_c
		printf("[%s:%d]: ", __FILE__, __LINE__);
		printf("D2U_UNICODE => Defined\n");

	#endif//#if MYDEBUG | DEBUG_dos2unix.c
	//////////////////////////////////////////////////debug


	pFlag->add_bom = 0;

//debug ===============================================
#if MYDEBUG | DEBUG_dos2unix_c
	printf("[%s:%d]: ", __FILE__, __LINE__);
	printf("ptr=%s\n", ptr);

#endif//#if MYDEBUG | DEBUG_dos2unix_c
//////////////////////////////////////////////////debug



	if ( ((ptr=strrchr(argv[0],'/')) == NULL) && ((ptr=strrchr(argv[0],'\\')) == NULL) )
		ptr = argv[0];
	else
		ptr++;

	if ((strcmpi("mac2unix", ptr) == 0) || (strcmpi("mac2unix.exe", ptr) == 0))
	{
		pFlag->FromToMode = FROMTO_MAC2UNIX;
		strcpy(progname,"mac2unix");
	}

	while ((++ArgIdx < argc) && (!ShouldExit))
	{
		/* is it an option? */
		if ((argv[ArgIdx][0] == '-') && process_options)
		{
			/* an option */
			if (strcmp(argv[ArgIdx],"--") == 0)
				process_options = 0;
			else if ((strcmp(argv[ArgIdx],"-h") == 0) || (strcmp(argv[ArgIdx],"--help") == 0))
			{
				PrintUsage(progname);
				return(pFlag->error);
			}
			else if ((strcmp(argv[ArgIdx],"-k") == 0) || (strcmp(argv[ArgIdx],"--keepdate") == 0))
				pFlag->KeepDate = 1;
			else if ((strcmp(argv[ArgIdx],"-f") == 0) || (strcmp(argv[ArgIdx],"--force") == 0))
				pFlag->Force = 1;
			else if ((strcmp(argv[ArgIdx],"-s") == 0) || (strcmp(argv[ArgIdx],"--safe") == 0))
				pFlag->Force = 0;
			else if ((strcmp(argv[ArgIdx],"-q") == 0) || (strcmp(argv[ArgIdx],"--quiet") == 0))
				pFlag->Quiet = 1;
			else if ((strcmp(argv[ArgIdx],"-l") == 0) || (strcmp(argv[ArgIdx],"--newline") == 0))
				pFlag->NewLine = 1;
			else if ((strcmp(argv[ArgIdx],"-m") == 0) || (strcmp(argv[ArgIdx],"--add-bom") == 0))
				pFlag->add_bom = 1;
			else if ((strcmp(argv[ArgIdx],"-S") == 0) || (strcmp(argv[ArgIdx],"--skip-symlink") == 0))
				pFlag->Follow = SYMLINK_SKIP;
			else if ((strcmp(argv[ArgIdx],"-F") == 0) || (strcmp(argv[ArgIdx],"--follow-symlink") == 0))
				pFlag->Follow = SYMLINK_FOLLOW;
			else if ((strcmp(argv[ArgIdx],"-R") == 0) || (strcmp(argv[ArgIdx],"--replace-symlink") == 0))
				pFlag->Follow = SYMLINK_REPLACE;
			else if ((strcmp(argv[ArgIdx],"-V") == 0) || (strcmp(argv[ArgIdx],"--version") == 0))
			{
				PrintVersion(progname);
				PrintLocaledir(localedir);
				return(pFlag->error);
			}
			else if ((strcmp(argv[ArgIdx],"-L") == 0) || (strcmp(argv[ArgIdx],"--license") == 0))
			{
				PrintLicense();
				return(pFlag->error);
			}
			else if (strcmp(argv[ArgIdx],"-ascii") == 0)  /* SunOS compatible options */
				pFlag->ConvMode = CONVMODE_ASCII;
			else if (strcmp(argv[ArgIdx],"-7") == 0)
				pFlag->ConvMode = CONVMODE_7BIT;
			else if (strcmp(argv[ArgIdx],"-iso") == 0)
			{
				pFlag->ConvMode = (int)query_con_codepage();
				if (!pFlag->Quiet)
				{
				   fprintf(stderr,"%s: ",progname);
				   fprintf(stderr,_("active code page: %d\n"), pFlag->ConvMode);
				}
				if (pFlag->ConvMode < 2)
				   pFlag->ConvMode = CONVMODE_437;
			}
			else if (strcmp(argv[ArgIdx],"-437") == 0)
				pFlag->ConvMode = CONVMODE_437;
			else if (strcmp(argv[ArgIdx],"-850") == 0)
				pFlag->ConvMode = CONVMODE_850;
			else if (strcmp(argv[ArgIdx],"-860") == 0)
				pFlag->ConvMode = CONVMODE_860;
			else if (strcmp(argv[ArgIdx],"-863") == 0)
				pFlag->ConvMode = CONVMODE_863;
			else if (strcmp(argv[ArgIdx],"-865") == 0)
				pFlag->ConvMode = CONVMODE_865;
			else if (strcmp(argv[ArgIdx],"-1252") == 0)
				pFlag->ConvMode = CONVMODE_1252;
			else if ((strcmp(argv[ArgIdx],"-c") == 0) || (strcmp(argv[ArgIdx],"--convmode") == 0))
			{
				if (++ArgIdx < argc)
				{
				  if (strcmpi(argv[ArgIdx],"ascii") == 0)  /* Benjamin Lin's legacy options */
				    pFlag->ConvMode = CONVMODE_ASCII;
				  else if (strcmpi(argv[ArgIdx], "7bit") == 0)
				    pFlag->ConvMode = CONVMODE_7BIT;
				  else if (strcmpi(argv[ArgIdx], "iso") == 0)
				  {
				    pFlag->ConvMode = (int)query_con_codepage();
				    if (!pFlag->Quiet)
				    {
				       fprintf(stderr,"%s: ",progname);
				       fprintf(stderr,_("active code page: %d\n"), pFlag->ConvMode);
				    }
				    if (pFlag->ConvMode < 2)
				       pFlag->ConvMode = CONVMODE_437;
				  }
				  else if (strcmpi(argv[ArgIdx], "mac") == 0)
				    pFlag->FromToMode = FROMTO_MAC2UNIX;
				  else
				  {
				    fprintf(stderr,"%s: ",progname);
				    fprintf(stderr, _("invalid %s conversion mode specified\n"),argv[ArgIdx]);
				    pFlag->error = 1;
				    ShouldExit = 1;
				    pFlag->stdio_mode = 0;
				  }
				}
				else
				{
				  ArgIdx--;
				  fprintf(stderr,"%s: ",progname);
				  fprintf(stderr,_("option '%s' requires an argument\n"),argv[ArgIdx]);
				  pFlag->error = 1;
				  ShouldExit = 1;
				  pFlag->stdio_mode = 0;
				}
			}

			else if ((strcmp(argv[ArgIdx],"-o") == 0) || (strcmp(argv[ArgIdx],"--oldfile") == 0))
			{
				/* last convert not paired */
				if (!CanSwitchFileMode)
				{
				  fprintf(stderr,"%s: ",progname);
				  fprintf(stderr, _("target of file %s not specified in new file mode\n"), argv[ArgIdx-1]);
				  pFlag->error = 1;
				  ShouldExit = 1;
				  pFlag->stdio_mode = 0;
				}
				pFlag->NewFile = 0;
			}

			else if ((strcmp(argv[ArgIdx],"-n") == 0) || (strcmp(argv[ArgIdx],"--newfile") == 0))
			{
				/* last convert not paired */
				if (!CanSwitchFileMode)
				{
				  fprintf(stderr,"%s: ",progname);
				  fprintf(stderr, _("target of file %s not specified in new file mode\n"), argv[ArgIdx-1]);
				  pFlag->error = 1;
				  ShouldExit = 1;
				  pFlag->stdio_mode = 0;
				}
				pFlag->NewFile = 1;
			}
			else { /* wrong option */
				PrintUsage(progname);
				ShouldExit = 1;
				pFlag->error = 1;
				pFlag->stdio_mode = 0;
			}
		}
		else
		{
			pFlag->stdio_mode = 0;
			/* not an option */
			if (pFlag->NewFile)
			{
				if (CanSwitchFileMode)
				  CanSwitchFileMode = 0;
				else
				{
				  RetVal = ConvertDosToUnixNewFile(argv[ArgIdx-1], argv[ArgIdx], pFlag, progname);
				  if (pFlag->status & NO_REGFILE)
				  {
				    if (!pFlag->Quiet)
				    {
				      fprintf(stderr,"%s: ",progname);
				      fprintf(stderr, _("Skipping %s, not a regular file.\n"), argv[ArgIdx-1]);
				    }
				  } else if (pFlag->status & OUTPUTFILE_SYMLINK)
				  {
				    if (!pFlag->Quiet)
				    {
				      fprintf(stderr,"%s: ",progname);
				      fprintf(stderr, _("Skipping %s, output file %s is a symbolic link.\n"), argv[ArgIdx-1], argv[ArgIdx]);
				    }
				  } else if (pFlag->status & INPUT_TARGET_NO_REGFILE)
				  {
				    if (!pFlag->Quiet)
				    {
				      fprintf(stderr,"%s: ",progname);
				      fprintf(stderr, _("Skipping symbolic link %s, target is not a regular file.\n"), argv[ArgIdx-1]);
				    }
				  } else if (pFlag->status & OUTPUT_TARGET_NO_REGFILE)
				  {
				    if (!pFlag->Quiet)
				    {
				      fprintf(stderr,"%s: ",progname);
				      fprintf(stderr, _("Skipping %s, target of symbolic link %s is not a regular file.\n"), argv[ArgIdx-1], argv[ArgIdx]);
				    }
				  } else if (pFlag->status & BINARY_FILE)
				  {
				    if (!pFlag->Quiet)
				    {
				      fprintf(stderr,"%s: ",progname);
				      fprintf(stderr, _("Skipping binary file %s\n"), argv[ArgIdx-1]);
				    }
				  } else if (pFlag->status & WRONG_CODEPAGE)
				  {
				    if (!pFlag->Quiet)
				    {
				      fprintf(stderr,"%s: ",progname);
				      fprintf(stderr, _("code page %d is not supported.\n"), pFlag->ConvMode);
				    }
				  } else if (pFlag->status & LOCALE_NOT_UTF8)
				  {
				    if (!pFlag->Quiet)
				    {
				      fprintf(stderr,"%s: ",progname);
				      fprintf(stderr, _("Skipping UTF-16 file %s, the current locale character encoding is not UTF-8.\n"), argv[ArgIdx-1]);
				    }
				  } else if (pFlag->status & WCHAR_T_TOO_SMALL)
				  {
				    if (!pFlag->Quiet)
				    {
				      fprintf(stderr,"%s: ",progname);
				      fprintf(stderr, _("Skipping UTF-16 file %s, the size of wchar_t is %d bytes.\n"), argv[ArgIdx-1], (int)sizeof(wchar_t));
				    }
				  } else if (pFlag->status & UNICODE_CONVERSION_ERROR)
				  {
				    if (!pFlag->Quiet)
				    {
				      fprintf(stderr,"%s: ",progname);
				      fprintf(stderr, _("Skipping UTF-16 file %s, an UTF-16 conversion error occurred.\n"), argv[ArgIdx-1]);
				    }
				  } else {
				    if (!pFlag->Quiet)
				    {
				      fprintf(stderr,"%s: ",progname);
				      fprintf(stderr, _("converting file %s to file %s in Unix format ...\n"), argv[ArgIdx-1], argv[ArgIdx]);
				    }
				    if (RetVal)
				    {
				      if (!pFlag->Quiet)
				      {
				        fprintf(stderr,"%s: ",progname);
				        fprintf(stderr, _("problems converting file %s to file %s\n"), argv[ArgIdx-1], argv[ArgIdx]);
				      }
				    }
				  }
				  CanSwitchFileMode = 1;
				}
			}
			else
			{
				RetVal = ConvertDosToUnixNewFile(argv[ArgIdx], argv[ArgIdx], pFlag, progname);
				if (pFlag->status & NO_REGFILE)
				{
				  if (!pFlag->Quiet)
				  {
				    fprintf(stderr,"%s: ",progname);
				    fprintf(stderr, _("Skipping %s, not a regular file.\n"), argv[ArgIdx]);
				  }
				} else if (pFlag->status & OUTPUTFILE_SYMLINK)
				{
				  if (!pFlag->Quiet)
				  {
				    fprintf(stderr,"%s: ",progname);
				    fprintf(stderr, _("Skipping symbolic link %s.\n"), argv[ArgIdx]);
				  }
				} else if (pFlag->status & INPUT_TARGET_NO_REGFILE)
				{
				  if (!pFlag->Quiet)
				  {
				    fprintf(stderr,"%s: ",progname);
				    fprintf(stderr, _("Skipping symbolic link %s, target is not a regular file.\n"), argv[ArgIdx]);
				  }
				} else if (pFlag->status & BINARY_FILE)
				{
				  if (!pFlag->Quiet)
				  {
				    fprintf(stderr,"%s: ",progname);
				    fprintf(stderr, _("Skipping binary file %s\n"), argv[ArgIdx]);
				  }
				} else if (pFlag->status & WRONG_CODEPAGE)
				{
				  if (!pFlag->Quiet)
				  {
				    fprintf(stderr,"%s: ",progname);
				    fprintf(stderr, _("code page %d is not supported.\n"), pFlag->ConvMode);
				  }
				} else if (pFlag->status & LOCALE_NOT_UTF8)
				{
				  if (!pFlag->Quiet)
				  {
				    fprintf(stderr,"%s: ",progname);
				    fprintf(stderr, _("Skipping UTF-16 file %s, the current locale character encoding is not UTF-8.\n"), argv[ArgIdx]);
				  }
				} else if (pFlag->status & WCHAR_T_TOO_SMALL)
				{
				  if (!pFlag->Quiet)
				  {
				    fprintf(stderr,"%s: ",progname);
				    fprintf(stderr, _("Skipping UTF-16 file %s, the size of wchar_t is %d bytes.\n"), argv[ArgIdx], (int)sizeof(wchar_t));
				  }
				} else if (pFlag->status & UNICODE_CONVERSION_ERROR)
				{
				  if (!pFlag->Quiet)
				  {
				    fprintf(stderr,"%s: ",progname);
				    fprintf(stderr, _("Skipping UTF-16 file %s, an UTF-16 conversion error occurred.\n"), argv[ArgIdx]);
				  }
				} else {
				  if (!pFlag->Quiet)
				  {
				    fprintf(stderr,"%s: ",progname);
				    fprintf(stderr, _("converting file %s to Unix format ...\n"), argv[ArgIdx]);
				  }
				  if (RetVal)
				  {
				    if (!pFlag->Quiet)
				    {
				      fprintf(stderr,"%s: ",progname);
				      fprintf(stderr, _("problems converting file %s\n"), argv[ArgIdx]);
				    }
				  }
				}
			}
		}
	}

	/* no file argument, use stdin and stdout */
	if (pFlag->stdio_mode)
	{
		exit(ConvertDosToUnixStdio(pFlag, progname));
	}


	if (!CanSwitchFileMode)
	{
		fprintf(stderr,"%s: ",progname);
		fprintf(stderr, _("target of file %s not specified in new file mode\n"), argv[ArgIdx-1]);
		pFlag->error = 1;
	}
	return (pFlag->error);
>>>>>>> B1_READ_unix2dos.c