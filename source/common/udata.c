/*
******************************************************************************
*
*   Copyright (C) 1999-2001, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  udata.c
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999oct25
*   created by: Markus W. Scherer
*/

#include "unicode/utypes.h"
#include "unicode/putil.h"
#include "umutex.h"
#include "cmemory.h"
#include "cstring.h"
#include "unicode/udata.h"
#include "unicode/uversion.h"
#include "uhash.h"

#ifdef OS390
#include <stdlib.h>
#endif

/* configuration ---------------------------------------------------------- */

#if !defined(HAVE_DLOPEN)
# define HAVE_DLOPEN 0
#endif

#if !defined(UDATA_DLL) && !defined(UDATA_MAP) &&!defined(UDATA_FILES)
#   define UDATA_DLL
#endif

#define COMMON_DATA_NAME U_ICUDATA_NAME
#define COMMON_DATA_NAME_LENGTH 8
/* Tests must verify that it remains 8 characters. */

#ifdef OS390
#define COMMON_DATA1_NAME U_ICUDATA_NAME"_390"
#define COMMON_DATA1_NAME_LENGTH (COMMON_DATA_NAME_LENGTH + 4)
static UBool s390dll = TRUE;
#endif

#define DATA_TYPE "dat"

/* If you are excruciatingly bored turn this on .. */
/* #define UDATA_DEBUG 1 */

#if defined(UDATA_DEBUG)
#   include <stdio.h>
#endif

/* DLL/shared library base functions ---------------------------------------- */

#ifdef WIN32
#   define WIN32_LEAN_AND_MEAN
#   define NOGDI
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX
#   include <windows.h>

    typedef HINSTANCE Library;

#   define LIB_SUFFIX ".dll"

#   define NO_LIBRARY NULL
#   define IS_LIBRARY(lib) ((lib)!=NULL)
#   define LOAD_LIBRARY(path, basename) LoadLibrary(path)
#   define UNLOAD_LIBRARY(lib) FreeLibrary(lib)

#   define GET_LIBRARY_ENTRY(lib, entryName) GetProcAddress(lib, entryName)

#elif HAVE_DLOPEN  /* POSIX-y shared library environment. dlopen() or equivalent.. */
#   ifndef UDATA_SO_SUFFIX
#       error Please define UDATA_SO_SUFFIX to the shlib suffix (i.e. '.so' )
#   endif

#   define LIB_PREFIX "lib"
#   define LIB_PREFIX_LENGTH 3
#   define LIB_SUFFIX UDATA_SO_SUFFIX

#   ifdef ICU_USE_SHL_LOAD  /* Some sort of compatibility stub:
                             * HPUX shl_load
                             * OS390 dllload */

#       define  RTLD_LAZY 0
#       define  RTLD_GLOBAL 0

#       ifdef OS390
#           include <dll.h>

#           define  RTLD_LAZY 0
#           define  RTLD_GLOBAL 0

            void *dlopen(const char *filename, int flag) {
                dllhandle *handle;

#               ifdef UDATA_DEBUG
                    fprintf(stderr, "dllload: %s ", filename);
#               endif
                handle=dllload(filename);
#               ifdef UDATA_DEBUG
                    fprintf(stderr, " -> %08X\n", handle );
#               endif
                    return handle;
            }

            void *dlsym(void *h, const char *symbol) {
                void *val=0;
                val=dllqueryvar((dllhandle*)h,symbol);
#               ifdef UDATA_DEBUG
                    fprintf(stderr, "dllqueryvar(%08X, %s) -> %08X\n", h, symbol, val);
#               endif
                return val;
            }

            int dlclose(void *handle) {
#               ifdef UDATA_DEBUG
                    fprintf(stderr, "dllfree: %08X\n", handle);
#               endif
                return dllfree((dllhandle*)handle);
            }
#       else /* not OS390:  HPUX shl_load  */
#           include <dl.h>

            void *dlopen (const char *filename, int flag) {
                void *handle; /* real type: 'shl_t' */
#               ifdef UDATA_DEBUG
                    fprintf(stderr, "shl_load: %s ", filename);
#               endif
                handle=shl_load(filename, BIND_NONFATAL | BIND_DEFERRED | DYNAMIC_PATH, 0L);
#               ifdef UDATA_DEBUG
                    fprintf(stderr, " -> %08X\n", handle );
#               endif
                return handle;
            }

            void *dlsym(void *h, char *symbol) {
                void *val=0;
                int rv;
                shl_t mysh;

                mysh=(shl_t)h; /* real type */

                rv=shl_findsym(&mysh, symbol, TYPE_DATA, (void*)&val);
#               ifdef UDATA_DEBUG
                    fprintf(stderr, "shl_findsym(%08X, %s) -> %08X [%d]\n", h, symbol, val, rv);
#               endif
                return val;
            }

            int dlclose (void *handle) {
#  ifndef HPUX
#               ifdef UDATA_DEBUG
                    fprintf(stderr, "shl_unload: %08X\n", handle);
#               endif
                return shl_unload((shl_t)handle);
#  else
            /* "shl_unload .. always unloads the library.. no refcount is kept on PA32"
               -- HPUX man pages [v. 11]

               Fine, we'll leak! [for now.. Jitterbug 414 has been filed ]
            */
            return 0;
#  endif /* HPUX */
            }
#       endif /* HPUX shl_load */
#   else /* not ICU_USE_SHL_LOAD */
        /* 'de facto standard' dlopen etc. */
#       include <dlfcn.h>
#ifndef RTLD_GLOBAL
#define RTLD_GLOBAL 0
#endif
#   endif

    typedef void *Library;

#   define NO_LIBRARY NULL
#   define IS_LIBRARY(lib) ((lib)!=NULL)


#ifndef UDATA_DEBUG
#   define LOAD_LIBRARY(path, basename) dlopen(path, RTLD_LAZY|RTLD_GLOBAL)
#   define UNLOAD_LIBRARY(lib) dlclose(lib)
#   define GET_LIBRARY_ENTRY(lib, entryName) dlsym(lib, entryName)
#else
  void *LOAD_LIBRARY(const char *path, const char *basename)
  {
    void *rc;
    rc = dlopen(path, RTLD_LAZY|RTLD_GLOBAL);
    fprintf(stderr, "Load [%s|%s] -> %p\n", path, basename, rc);
    return rc;
  }
  void  UNLOAD_LIBRARY(void *lib)
  {
    dlclose(lib);
    fprintf(stderr, "Unload [%p]\n", lib);
  }
  void * GET_LIBRARY_ENTRY(void *lib, const char *entryName)
  {
    void *rc;
    rc = dlsym(lib, entryName);
    fprintf(stderr, "Get[%p] %s->%p\n", lib, entryName, rc);
    return rc;
  }
#endif
  /* End of dlopen or compatible functions */

#else /* unknown platform, no DLL implementation */
#   ifndef UDATA_NO_DLL
#       define UDATA_NO_DLL 1
#   endif
    typedef const void *Library;

#   define NO_LIBRARY NULL
#   define IS_LIBRARY(lib) ((lib)!=NULL)
#   define LOAD_LIBRARY(path, basename) NULL
#   define UNLOAD_LIBRARY(lib) ;

#   define GET_LIBRARY_ENTRY(lib, entryName) NULL

#endif

/* memory-mapping base definitions ------------------------------------------ */

/* we need these definitions before the common ones because
   MemoryMap is a field of UDataMemory;
   however, the mapping functions use UDataMemory,
   therefore they are defined later
 */

#define MAP_WIN32       1
#define MAP_POSIX       2
#define MAP_FILE_STREAM 3

#ifdef WIN32
    typedef HANDLE MemoryMap;

#   define NO_MAP NULL
#   define IS_MAP(map) ((map)!=NULL)

#   define MAP_IMPLEMENTATION MAP_WIN32

/* ### Todo: auto detect mmap(). Until then, just add your platform here. */
#elif HAVE_MMAP || defined(U_LINUX) || defined(POSIX) || defined(U_SOLARIS) || defined(AIX) || defined(HPUX) || defined(OS390) || defined(PTX)
    typedef size_t MemoryMap;

#   define NO_MAP 0
#   define IS_MAP(map) ((map)!=0)

#   include <unistd.h>
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <fcntl.h>

#   ifndef MAP_FAILED
#       define MAP_FAILED ((void*)-1)
#   endif

#   define MAP_IMPLEMENTATION MAP_POSIX

#else /* unknown platform, no memory map implementation: use FileStream/uprv_malloc() instead */

#   include "filestrm.h"

    typedef void *MemoryMap;

#   define NO_MAP NULL
#   define IS_MAP(map) ((map)!=NULL)

#   define MAP_IMPLEMENTATION MAP_FILE_STREAM

#endif

/* common definitions ------------------------------------------------------- */

/* constants for UDataMemory flags: type of data memory */
enum {
    NO_DATA_MEMORY   = 0,
    FLAT_DATA_MEMORY = 1,
    DLL_DATA_MEMORY  = 2
};
#define DATA_MEMORY_TYPE_MASK    0xf

/* constants for UDataMemory flags: type of TOC */
enum {
    NO_TOC              =0x00,
    OFFSET_TOC          =0x10,
    POINTER_TOC         =0x20,
    DLL_INTRINSIC_TOC   =0x30
};
#define TOC_TYPE_MASK            0xf0

/* constants for UDataMemory flags: type of TOC */
#define SET_DATA_POINTER_FLAG     0x40000000
#define DYNAMIC_DATA_MEMORY_FLAG  0x80000000

#define IS_DATA_MEMORY_LOADED(pData) ((pData)->flags!=0)


typedef struct  {
    uint16_t    headerSize;
    uint8_t     magic1;
    uint8_t     magic2;
} MappedData;

typedef struct  {
    MappedData  dataHeader;
    UDataInfo   info;
} DataHeader;

typedef const DataHeader *
LookupFn(const UDataMemory *pData,
         const char *tocEntryName,
         const char *dllEntryName,
         UErrorCode *pErrorCode);

/*----------------------------------------------------------------------------------*
 *                                                                                  *
 *  UDataMemory     Very Important Struct.  Pointers to these are returned          *
 *                  to callers from the various data open functions.                *
 *                  These keep track of everything about the memeory                *
 *                                                                                  *
 *----------------------------------------------------------------------------------*/
typedef struct UDataMemory {
    UDataMemory      *parent;      /*  Set if we're suballocated from some common     */
    Library           lib;         /* OS dependent handle for DLLs, .so, etc          */
    MemoryMap         map;         /* Handle, or whatever.  OS dependent.             */
    LookupFn         *lookupFn;
    const void       *toc;         /* For common memory, to find pieces within.       */
    const DataHeader *pHeader;     /* Header.  For common data, header is at top of file */
    uint32_t          flags;       /* Memory format, TOC type, Allocation type, etc.  */
    int32_t           refCount;    /* Not used just yet...                            */
} UDataMemory;


static void UDataMemory_init(UDataMemory *This) {  
    uprv_memset(This, 0, sizeof(UDataMemory));
}


static void UDataMemory_copy(UDataMemory *dest, UDataMemory *source) {
    uprv_memcpy(dest, source, sizeof(UDataMemory));
}

static UDataMemory *UDataMemory_createNewInstance() {
    UDataMemory *This;
    This = uprv_malloc(sizeof(UDataMemory));
    UDataMemory_init(This);
    return This;
}

/*----------------------------------------------------------------------------------*
 *                                                                                  *
 *  Pointer TOCs.   This form of table-of-contents should be removed because        *
 *                  DLLs must be relocated on loading to correct the pointer values *
 *                  and this operation makes shared memory mapping of the data      *
 *                  much less likely to work.                                       *
 *                                                                                  *
 *----------------------------------------------------------------------------------*/
typedef struct {
    const char *entryName;
    const DataHeader *pHeader;
} PointerTOCEntry;


/*----------------------------------------------------------------------------------*
 *                                                                                  *
 *   Memory Mapped File support.  Platform dependent implementation of functions    *
 *                                used by the rest of the implementation.           *
 *                                                                                  *
 *----------------------------------------------------------------------------------*/
#if MAP_IMPLEMENTATION==MAP_WIN32
    static UBool
    uprv_mapFile(
         UDataMemory *pData,    /* Fill in with info on the result doing the mapping. */
                                /*   Output only; any original contents are cleared.  */
         const char *path       /* File path to be opened/mapped                      */
         ) 
    {
        HANDLE map;
        HANDLE file;
        
        UDataMemory_init(pData); /* Clear the output struct.        */

        /* open the input file */
        file=CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL|FILE_FLAG_RANDOM_ACCESS, NULL);
        if(file==INVALID_HANDLE_VALUE) {
            return FALSE;
        }
        
        /* create an unnamed Windows file-mapping object for the specified file */
        map=CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, NULL);
        CloseHandle(file);
        if(map==NULL) {
            return FALSE;
        }
        
        /* map a view of the file into our address space */
        pData->pHeader=(const DataHeader *)MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0);
        if(pData->pHeader==NULL) {
            CloseHandle(map);
            return FALSE;
        }
        pData->map=map;
        pData->flags=FLAT_DATA_MEMORY;
        return TRUE;
    }


    static void
    uprv_unmapFile(UDataMemory *pData) {
        if(pData!=NULL && pData->map!=NULL) {
            UnmapViewOfFile(pData->pHeader);
            CloseHandle(pData->map);
            pData->pHeader=NULL;
            pData->map=NULL;
        }
    }

#elif MAP_IMPLEMENTATION==MAP_POSIX
    static UBool
    uprv_mapFile(UDataMemory *pData, const char *path) {
        int fd;
        int length;
        struct stat mystat;
        const void *data;

        UDataMemory_init(pData); /* Clear the output struct.        */

        /* determine the length of the file */
        if(stat(path, &mystat)!=0 || mystat.st_size<=0) {
            return FALSE;
        }
        length=mystat.st_size;

        /* open the file */
        fd=open(path, O_RDONLY);
        if(fd==-1) {
            return FALSE;
        }

        /* get a view of the mapping */
#ifndef HPUX
        data=mmap(0, length, PROT_READ, MAP_SHARED,  fd, 0);
#else
        data=mmap(0, length, PROT_READ, MAP_PRIVATE, fd, 0);
#endif
        close(fd); /* no longer needed */
        if(data==MAP_FAILED) {

#       ifdef UDATA_DEBUG
              perror("mmap");
#       endif

            return FALSE;
        }

#       ifdef UDATA_DEBUG
            fprintf(stderr, "mmap of %s [%d bytes] succeeded, -> 0x%X\n", path, length, data);
            fflush(stderr);
#       endif

        pData->map=length;
        pData->pHeader=(const DataHeader *)data;
        pData->flags=FLAT_DATA_MEMORY;
        return TRUE;
    }

    static void
    uprv_unmapFile(UDataMemory *pData) {
        if(pData!=NULL && pData->map>0) {
            if(munmap((void *)pData->pHeader, pData->map)==-1) {
#               ifdef UDATA_DEBUG
                    perror("munmap");
#               endif
            }
            pData->pHeader=NULL;
            pData->map=0;
        }
    }

#elif MAP_IMPLEMENTATION==MAP_FILE_STREAM
    static UBool
    uprv_mapFile(UDataMemory *pData, const char *path) {
        FileStream *file;
        int32_t fileLength;
        void *p;

        UDataMemory_init(pData); /* Clear the output struct.        */
        /* open the input file */
        file=T_FileStream_open(path, "rb");
        if(file==NULL) {
            return FALSE;
        }

        /* get the file length */
        fileLength=T_FileStream_size(file);
        if(T_FileStream_error(file) || fileLength<=20) {
            T_FileStream_close(file);
            return FALSE;
        }

        /* allocate the data structure */
        p=uprv_malloc(fileLength);
        if(p==NULL) {
            T_FileStream_close(file);
            return FALSE;
        }

        /* read the file */
        if(fileLength!=T_FileStream_read(file, p, fileLength)) {
            uprv_free(p);
            T_FileStream_close(file);
            return FALSE;
        }

        T_FileStream_close(file);
        pData->map=p;
        pData->pHeader=(const DataHeader *)p;
        pData->flags=FLAT_DATA_MEMORY;
        return TRUE;
    }

    static void
    uprv_unmapFile(UDataMemory *pData) {
        if(pData!=NULL && pData->map!=NULL) {
            uprv_free(pData->map);
            pData->map=NULL;
        }
    }

#else
#   error MAP_IMPLEMENTATION is set incorrectly
#endif


    
/*----------------------------------------------------------------------------------*
 *                                                                                  *
 *    entry point lookup implementations                                            *
 *                                                                                  *
 *----------------------------------------------------------------------------------*/
static const DataHeader *
normalizeDataPointer(const DataHeader *p) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    if(p==NULL || (p->dataHeader.magic1==0xda && p->dataHeader.magic2==0x27)) {
        return p;
    } else {
        return (const DataHeader *)((const double *)p+1);
    }
}

static const DataHeader *
offsetTOCLookupFn(const UDataMemory *pData,
                  const char *tocEntryName,
                  const char *dllEntryName,
                  UErrorCode *pErrorCode) {

#ifdef UDATA_DEBUG
  fprintf(stderr, "offsetTOC[%p] looking for %s/%s\n",
      pData,
      tocEntryName,dllEntryName);
#endif

    if(pData->toc!=NULL) {
        const char *base=(const char *)pData->toc;
        uint32_t *toc=(uint32_t *)pData->toc;
        uint32_t start, limit, number;

        /* perform a binary search for the data in the common data's table of contents */
        start=0;
        limit=*toc++;   /* number of names in this table of contents */
        while(start<limit-1) {
            number=(start+limit)/2;
            if(uprv_strcmp(tocEntryName, base+toc[2*number])<0) {
                limit=number;
            } else {
                start=number;
            }
        }

        if(uprv_strcmp(tocEntryName, base+toc[2*start])==0) {
            /* found it */
#ifdef UDATA_DEBUG
      fprintf(stderr, "Found: %p\n",(base+toc[2*start+1]));
#endif
            return (const DataHeader *)(base+toc[2*start+1]);
        } else {
#ifdef UDATA_DEBUG
      fprintf(stderr, "Not found.\n");
#endif
            return NULL;
        }
    } else {
#ifdef UDATA_DEBUG
        fprintf(stderr, "returning header\n");
#endif

        return pData->pHeader;
    }
}

static const DataHeader *
pointerTOCLookupFn(const UDataMemory *pData,
                   const char *tocEntryName,
                   const char *dllEntryName,
                   UErrorCode *pErrorCode) {
#ifdef UDATA_DEBUG
  fprintf(stderr, "ptrTOC[%p] looking for %s/%s\n",
      pData,
      tocEntryName,dllEntryName);
#endif
    if(pData->toc!=NULL) {
        const PointerTOCEntry *toc=(const PointerTOCEntry *)((const uint32_t *)pData->toc+2);
        uint32_t start, limit, number;

        /* perform a binary search for the data in the common data's table of contents */
        start=0;
        limit=*(const uint32_t *)pData->toc; /* number of names in this table of contents */

#ifdef UDATA_DEBUG
        fprintf(stderr, "  # of ents: %d\n", limit);
        fflush(stderr);
#endif

        if (limit == 0) {         /* Stub common data library used during build is empty. */
            return NULL;
        }

        while(start<limit-1) {
            number=(start+limit)/2;
            if(uprv_strcmp(tocEntryName, toc[number].entryName)<0) {
                limit=number;
            } else {
                start=number;
            }
        }

        if(uprv_strcmp(tocEntryName, toc[start].entryName)==0) {
            /* found it */
#ifdef UDATA_DEBUG
            fprintf(stderr, "FOUND: %p\n",
                normalizeDataPointer(toc[start].pHeader));
#endif

            return normalizeDataPointer(toc[start].pHeader);
        } else {
#ifdef UDATA_DEBUG
            fprintf(stderr, "NOT found\n");
#endif
            return NULL;
        }
    } else {
#ifdef UDATA_DEBUG
        fprintf(stderr, "Returning header\n");
#endif
        return pData->pHeader;
    }
}

static const DataHeader *
dllTOCLookupFn(const UDataMemory *pData,
               const char *tocEntryName,
               const char *dllEntryName,
               UErrorCode *pErrorCode) {
#ifndef UDATA_NO_DLL
    if(pData->lib!=NULL) {
        return normalizeDataPointer((const DataHeader *)GET_LIBRARY_ENTRY(pData->lib, dllEntryName));
    }
#endif
    return NULL;
}

/* common library functions ------------------------------------------------- */

static UDataMemory commonICUData={ NULL };

static void
setCommonICUData(UDataMemory *pData) {
    UBool setThisLib=FALSE;

    /* in the mutex block, set the common library for this process */
    umtx_lock(NULL);
    if(!IS_DATA_MEMORY_LOADED(&commonICUData)) {
        uprv_memcpy(&commonICUData, pData, sizeof(UDataMemory));
        commonICUData.flags&=~DYNAMIC_DATA_MEMORY_FLAG;
        uprv_memset(pData, 0, sizeof(UDataMemory));
        setThisLib=TRUE;
    }
    umtx_unlock(NULL);

    /* if a different thread set it first, then free the extra library instance */
    if(!setThisLib) {
        udata_close(pData);
    }
}

static char *
strcpy_returnEnd(char *dest, const char *src) {
    while((*dest=*src)!=0) {
        ++dest;
        ++src;
    }
    return dest;
}

/*------------------------------------------------------------------------------*
 *                                                                              *
 *  setPathGetBasename   given a (possibly partial) path of an item             *
 *                       to be opened, compute a full directory path and leave  *
 *                       it in pathBuffer.  Returns a pointer to the null at    *
 *                       the end of the computed path.                          *
 *                       If a non-empty path buffer is passed in, assume that   *
 *                       it already contains the right result, and don't        *
 *                       bother to figure it all out again.                     *
 *                                                                              *
 *------------------------------------------------------------------------------*/
static char *
setPathGetBasename(const char *path, char *pathBuffer) {
    if(*pathBuffer!=0) {
        /* the pathBuffer is already filled,
           we just need to find the basename position;
           assume that the last filling function removed the basename from the buffer */
        return pathBuffer+uprv_strlen(pathBuffer);
    } else if(path==NULL) {
        /* copy the ICU_DATA path to the path buffer */
        path=u_getDataDirectory();
        if(path!=NULL && *path!=0) {
            return strcpy_returnEnd(pathBuffer, path);
        } else {
            /* there is no path */
            return pathBuffer;
        }
    } else {
        /* find the last file sepator in the input path */
        char *basename=uprv_strrchr(path, U_FILE_SEP_CHAR);
        if(basename==NULL) {
            /* copy the ICU_DATA path to the path buffer */
            path=u_getDataDirectory();
            if(path!=NULL && *path!=0) {
                return strcpy_returnEnd(pathBuffer, path);
            } else {
                /* there is no path */
                return pathBuffer;
            }
        } else {
            /* copy the path to the path buffer */
            ++basename;
            uprv_memcpy(pathBuffer, path, basename-path);
            basename=pathBuffer+(basename-path);
            *basename=0;
            return basename;
        }
    }
}

static const char *
findBasename(const char *path) {
    const char *basename=uprv_strrchr(path, U_FILE_SEP_CHAR);
    if(basename==NULL) {
        return path;
    } else {
        return basename+1;
    }
}


/*----------------------------------------------------------------------*
 *                                                                      *
 *   Cache for common data                                              *
 *      Functions for looking up or adding entries to a cache of        *
 *      data that has been previously opened.  Avoids a potentially     *
 *      expensive operation of re-opening the data for subsequent       *
 *      uses.                                                           *
 *                                                                      *
 *      Data remains cached for the duration of the process.            *
 *                                                                      *
 *----------------------------------------------------------------------*/

typedef struct DataCacheElement {
    char          *path;
    UDataMemory    item;
} DataCacheElement;
 
 /*   udata_getCacheHashTable()
 *     Get the hash table used to store the data cache entries.
 *     Lazy create it if it doesn't yet exist.
 */  
static UHashtable *udata_getHashTable() {
    static UHashtable *gHashTable;
    UErrorCode err = U_ZERO_ERROR;

    if (gHashTable != NULL) {
        return gHashTable;
    }
    umtx_lock(NULL); 
    if (gHashTable == NULL) {
        gHashTable = uhash_open(uhash_hashChars, uhash_compareChars, &err);
    }
    umtx_unlock(NULL);

    if U_FAILURE(err) {
        return NULL;
    }
    return gHashTable;
}



static UDataMemory *udata_findCachedData(const char *path)
{
    UHashtable   *htable;
    UDataMemory  *retVal;

    umtx_lock(NULL); 
    htable = udata_getHashTable();
    umtx_unlock(NULL);
    retVal = (UDataMemory *)uhash_get(htable, path);
    return retVal;
}


static UDataMemory *udata_cacheDataItem(const char *path, UDataMemory *item) {
    DataCacheElement *newElement;
    int               pathLen;
    UErrorCode        err = U_ZERO_ERROR;

    /* Create a new DataCacheElement - the thingy we store in the hash table -
     * and copy the supplied path and UDataMemoryItems into it.
     */
    newElement = uprv_malloc(sizeof(DataCacheElement));
    UDataMemory_copy(&newElement->item, item);
    newElement->item.flags &= ~DYNAMIC_DATA_MEMORY_FLAG;
    pathLen = uprv_strlen(path);
    newElement->path = uprv_malloc(pathLen+1);
    uprv_strcpy(newElement->path, path);

    /* Stick the new DataCacheElement into the hash table.
    */
    umtx_lock(NULL); 
    uhash_put(udata_getHashTable(), 
        newElement->path,               /* Key   */
        &newElement->item,              /* Value */
        &err);
    umtx_unlock(NULL);
              /*  Just ignore any error returned.
               *  1.  Failure to add to the cache is not, by itself, fatal.
               *  2.  The only reason the hash table would fail is if memory
               *      allocation fails, which means complete death is just
               *      around the corner anyway...
               */

    return &newElement->item;
}



/*                                                                     */
/*  Add a static reference to the common data from a library if the    */
/*      build options are set to request it.                           */
/*   Unless overridden by an explicit u_setCommonData, this will be    */
/*      our common data.                                               */
#if defined(UDATA_STATIC_LIB) || defined(UDATA_DLL)
extern  const DataHeader U_IMPORT U_ICUDATA_ENTRY_POINT;
#endif


/*----------------------------------------------------------------------*
 *                                                                      *
 *   openCommonData   Attempt to open a common format (.dat) file       *
 *                    Map it into memory (if it's not there already)    *
 *                    and return a UDataMemory object for it.           *
 *                    The UDataMemory object will either be heap or     *
 *                    global - in either case, it is permanent and can  *
 *                    be safely passed back the chain of callers.       *
 *                                                                      *
 *----------------------------------------------------------------------*/
static UDataMemory *
openCommonData(
               const char *path,          /*  Path from OpenCHoice?          */
               UBool isICUData,           /*  ICU Data true if path == NULL  */
               UErrorCode *pErrorCode)
 {
    const char *inBasename;
    char *basename, *suffix;
    const DataHeader *pHeader;
    char pathBuffer[1024];
    UDataMemory   tData;

    UDataMemory_init(&tData);


    /* "mini-cache" for common ICU data */
    if(isICUData && IS_DATA_MEMORY_LOADED(&commonICUData)) {
        return &commonICUData;
    }


    /* Is this data cached?  Meaning, have we seen this before  */
    if (!isICUData) {
        UDataMemory  *dataToReturn = udata_findCachedData(path);
        if (dataToReturn != NULL) {
            return dataToReturn;
        }
    }


    if (isICUData) {
        /* ICU common data is already in our address space, thanks to a static reference */
        /*   to a symbol from the data library.  Just follow that pointer to set up      */
        /*   our ICU data.                                                               */
        pHeader = &U_ICUDATA_ENTRY_POINT;
        if(!(pHeader->dataHeader.magic1==0xda && pHeader->dataHeader.magic2==0x27 &&
            pHeader->info.isBigEndian==U_IS_BIG_ENDIAN &&
            pHeader->info.charsetFamily==U_CHARSET_FAMILY)
            ) {
            /* header not valid */
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            return NULL;
        }

        if(pHeader->info.dataFormat[0]==0x43 &&
            pHeader->info.dataFormat[1]==0x6d &&
            pHeader->info.dataFormat[2]==0x6e &&
            pHeader->info.dataFormat[3]==0x44 &&
            pHeader->info.formatVersion[0]==1
            ) {
            /* dataFormat="CmnD" */
            tData.lib=NULL;
            tData.lookupFn=offsetTOCLookupFn;
            tData.toc=(const char *)pHeader+pHeader->dataHeader.headerSize;
            tData.flags= DLL_DATA_MEMORY | OFFSET_TOC;
        } else if(pHeader->info.dataFormat[0]==0x54 &&
            pHeader->info.dataFormat[1]==0x6f &&
            pHeader->info.dataFormat[2]==0x43 &&
            pHeader->info.dataFormat[3]==0x50 &&
            pHeader->info.formatVersion[0]==1
            ) {
            /* dataFormat="ToCP" */
            tData.lib=NULL;
            tData.lookupFn=pointerTOCLookupFn;
            tData.toc=(const char *)pHeader+pHeader->dataHeader.headerSize;
            tData.flags=DLL_DATA_MEMORY|POINTER_TOC;
        } else {
            /* dataFormat not recognized */
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            return NULL;
        }

        /* we have common data from a DLL */
        setCommonICUData(&tData);
        return &commonICUData;
    }



    /* set up path and basename */
    pathBuffer[0] = 0;
    basename=setPathGetBasename(path, pathBuffer);
    if(isICUData) {
        inBasename=COMMON_DATA_NAME;
    } else {
        inBasename=findBasename(path);
        if(*inBasename==0) {
            /* no basename, no common data */
            *pErrorCode=U_FILE_ACCESS_ERROR;
            return NULL;
        }
    }



    /* try to map a common data file */

    /* set up the file name */
    suffix=strcpy_returnEnd(basename, inBasename);
    uprv_strcpy(suffix, "." DATA_TYPE);      /*  DATA_TYPE is ".dat" */

    /* try path/basename first, then basename only */
    if( uprv_mapFile(&tData, pathBuffer) ||
        (basename!=pathBuffer && uprv_mapFile(&tData, basename))
        ) {
        *basename=0;

        /* we have mapped a file, check its header */
        pHeader=tData.pHeader;
        if(!(pHeader->dataHeader.magic1==0xda && pHeader->dataHeader.magic2==0x27 &&
            pHeader->info.isBigEndian==U_IS_BIG_ENDIAN &&
            pHeader->info.charsetFamily==U_CHARSET_FAMILY &&
            pHeader->info.dataFormat[0]==0x43 &&
            pHeader->info.dataFormat[1]==0x6d &&
            pHeader->info.dataFormat[2]==0x6e &&
            pHeader->info.dataFormat[3]==0x44 &&
            pHeader->info.formatVersion[0]==1)
            ) {
            uprv_unmapFile(&tData);
            tData.flags=0;
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            return NULL;
        }

        /* we have common data from a mapped file */
        tData.lookupFn=offsetTOCLookupFn;
        tData.toc=(const char *)pHeader+pHeader->dataHeader.headerSize;
        tData.flags|=OFFSET_TOC;
        if(isICUData) {
            setCommonICUData(&tData);
            return &commonICUData;
        } else {
            // Cache the UDataMemory struct for this .dat file,
            //   so we won't need to hunt it down and open it again next time
            //   something is needed from it.
            return udata_cacheDataItem(path, &tData);
        }
    }

    /* cleanup for the caller, to keep setPathGetBasename() consistent */
    *basename=0;

    /* no common data */
    *pErrorCode=U_FILE_ACCESS_ERROR;
    return NULL;
}

U_CAPI void U_EXPORT2
udata_setCommonData(const void *data, UErrorCode *pErrorCode) {
    if(pErrorCode==NULL || U_FAILURE(*pErrorCode)) {
        return;
    }

    if(data==NULL) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }

    /* do we already have common ICU data set? */
    if(IS_DATA_MEMORY_LOADED(&commonICUData)) {
        *pErrorCode=U_USING_DEFAULT_ERROR;
        return;
    } else {
        /* normalize the data pointer and test for validity */
        UDataMemory dataMemory={ NULL };
        const DataHeader *pHeader=normalizeDataPointer((const DataHeader *)data);
        if(!(pHeader->dataHeader.magic1==0xda && pHeader->dataHeader.magic2==0x27 &&
             pHeader->info.isBigEndian==U_IS_BIG_ENDIAN &&
             pHeader->info.charsetFamily==U_CHARSET_FAMILY)
        ) {
            /* header not valid */
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            return;

            /* which TOC type? */
        } else if(pHeader->info.dataFormat[0]==0x43 &&
                  pHeader->info.dataFormat[1]==0x6d &&
                  pHeader->info.dataFormat[2]==0x6e &&
                  pHeader->info.dataFormat[3]==0x44 &&
                  pHeader->info.formatVersion[0]==1
        ) {
            /* dataFormat="CmnD" */
            dataMemory.lookupFn=offsetTOCLookupFn;
            dataMemory.toc=(const char *)pHeader+pHeader->dataHeader.headerSize;
            dataMemory.flags=DLL_DATA_MEMORY|OFFSET_TOC;
        } else if(pHeader->info.dataFormat[0]==0x54 &&
                  pHeader->info.dataFormat[1]==0x6f &&
                  pHeader->info.dataFormat[2]==0x43 &&
                  pHeader->info.dataFormat[3]==0x50 &&
                  pHeader->info.formatVersion[0]==1
        ) {
            /* dataFormat="ToCP" */
            dataMemory.lookupFn=pointerTOCLookupFn;
            dataMemory.toc=(const char *)pHeader+pHeader->dataHeader.headerSize;
            dataMemory.flags=DLL_DATA_MEMORY|POINTER_TOC;
        } else {
            /* dataFormat not recognized */
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            return;
        }

        /* we have common data */
        dataMemory.flags|=SET_DATA_POINTER_FLAG;
        setCommonICUData(&dataMemory);
        if(dataMemory.flags!=0) {
            /* some thread passed us */
            *pErrorCode=U_USING_DEFAULT_ERROR;
        }
    }
}




/*---------------------------------------------------------------------------
 *
 *  udata_setAppData
 *
 *---------------------------------------------------------------------------- */

U_CAPI void U_EXPORT2 
udata_setAppData(const char *path, const void *data, UErrorCode *err)
{
};


/* main data loading function ----------------------------------------------- */

static void
setEntryNames(const char *type, const char *name,
              char *tocEntryName, char *dllEntryName) {
    while(*name!=0) {
        *tocEntryName=*name;
        if(*name=='.' || *name=='-') {
            *dllEntryName='_';
        } else {
            *dllEntryName=*name;
        }
        ++tocEntryName;
        ++dllEntryName;
        ++name;
    }

    if(type!=NULL && *type!=0) {
        *tocEntryName++='.';
        *dllEntryName++='_';
        do {
            *tocEntryName++=*dllEntryName++=*type++;
        } while(*type!=0);
    }

    *tocEntryName=*dllEntryName=0;
}

static UDataMemory *
doOpenChoice(const char *path, const char *type, const char *name,
             UDataMemoryIsAcceptable *isAcceptable, void *context,
             UErrorCode *pErrorCode) {
    char pathBuffer[1024];
    char tocEntryName[100], dllEntryName[100];
    UDataMemory dataMemory;
    UDataMemory *pCommonData, *pEntryData;
    const DataHeader *pHeader;
    const char *inBasename;
    char *basename, *suffix;
    UErrorCode errorCode=U_ZERO_ERROR;
    UBool isICUData= (UBool)(path==NULL);

    /* set up the ToC names for DLL and offset-ToC lookups */
    setEntryNames(type, name, tocEntryName, dllEntryName);


    /* try to get common data */
    pathBuffer[0]=0;
    pCommonData=openCommonData(path, isICUData, &errorCode);
#ifdef UDATA_DEBUG
    fprintf(stderr, "commonData;%p\n", pCommonData);
    fflush(stderr);
#endif

    if(U_SUCCESS(errorCode)) {
        /* look up the data piece in the common data */
        pHeader=pCommonData->lookupFn(pCommonData, tocEntryName, dllEntryName, &errorCode);
#ifdef UDATA_DEBUG
        fprintf(stderr, "Common found: %p\n", pHeader);
#endif
        if(pHeader!=NULL) {
            /* data found in the common data, test it */
            if(pHeader->dataHeader.magic1==0xda && pHeader->dataHeader.magic2==0x27 &&
                pHeader->info.isBigEndian==U_IS_BIG_ENDIAN &&
                (isAcceptable==NULL || isAcceptable(context, type, name, &pHeader->info))
                ) {
                pEntryData=UDataMemory_createNewInstance();
                if(pEntryData==NULL) {
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    return NULL;
                }
                UDataMemory_init(pEntryData);
                pEntryData->parent  = pCommonData;
                pEntryData->pHeader = pHeader;
                pEntryData->flags   = (pCommonData->flags&DATA_MEMORY_TYPE_MASK)|DYNAMIC_DATA_MEMORY_FLAG;
#ifdef UDATA_DEBUG
                fprintf(stderr, " made data @%p\n", pEntryData);
#endif
                return pEntryData;
            } else {
                /* the data is not acceptable, look further */
                /* If we eventually find something good, this errorcode will be */
                /*    cleared out.                                              */
                errorCode=U_INVALID_FORMAT_ERROR;
            }
        }

        /* the data was not found in the common data,  look further */
    }

    /* try to get an individual data file */
    basename=setPathGetBasename(path, pathBuffer);
    if(isICUData) {
        inBasename=COMMON_DATA_NAME;
    } else {
        inBasename=findBasename(path);
    }

#ifdef UDATA_DEBUG
    fprintf(stderr, "looking for ind. file\n");
#endif

    /* try path+basename+"_"+entryName first */
    if(*inBasename!=0) {
        suffix=strcpy_returnEnd(basename, inBasename);
        *suffix++='_';
        uprv_strcpy(suffix, tocEntryName);

        if( uprv_mapFile(&dataMemory, pathBuffer) ||
            (basename!=pathBuffer && uprv_mapFile(&dataMemory, basename))
        ) {
            pHeader=dataMemory.pHeader;
            if(pHeader->dataHeader.magic1==0xda && pHeader->dataHeader.magic2==0x27 &&
               pHeader->info.isBigEndian==U_IS_BIG_ENDIAN &&
               (isAcceptable==NULL || isAcceptable(context, type, name, &pHeader->info))
            ) {
                /* acceptable */
                pEntryData=(UDataMemory *)uprv_malloc(sizeof(UDataMemory));
                if(pEntryData==NULL) {
                    uprv_unmapFile(&dataMemory);
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    return NULL;
                }
                dataMemory.flags |= DYNAMIC_DATA_MEMORY_FLAG;
                uprv_memcpy(pEntryData, &dataMemory, sizeof(UDataMemory));
                return pEntryData;
            } else {
                /* the data is not acceptable, look further */
                uprv_unmapFile(&dataMemory);
                errorCode=U_INVALID_FORMAT_ERROR;
            }
        }
    }

    /* try path+entryName next */
    uprv_strcpy(basename, tocEntryName);
    if( uprv_mapFile(&dataMemory, pathBuffer) ||
        (basename!=pathBuffer && uprv_mapFile(&dataMemory, basename))
    ) {
        pHeader=dataMemory.pHeader;
        if(pHeader->dataHeader.magic1==0xda && pHeader->dataHeader.magic2==0x27 &&
           pHeader->info.isBigEndian==U_IS_BIG_ENDIAN &&
           (isAcceptable==NULL || isAcceptable(context, type, name, &pHeader->info))
        ) {
            /* acceptable */
            pEntryData=(UDataMemory *)uprv_malloc(sizeof(UDataMemory));
            if(pEntryData==NULL) {
                uprv_unmapFile(&dataMemory);
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                return NULL;
            }
            dataMemory.flags |= DYNAMIC_DATA_MEMORY_FLAG;
            uprv_memcpy(pEntryData, &dataMemory, sizeof(UDataMemory));
            return pEntryData;
        } else {
            /* the data is not acceptable, look further */
            uprv_unmapFile(&dataMemory);
            errorCode=U_INVALID_FORMAT_ERROR;
        }
    }

    /* data not found */
    if(U_SUCCESS(*pErrorCode)) {
        if(U_SUCCESS(errorCode)) {
            /* file not found */
            *pErrorCode=U_FILE_ACCESS_ERROR;
        } else {
            /* entry point not found or rejected */
            *pErrorCode=errorCode;
        }
    }
    return NULL;
}

static void
unloadDataMemory(UDataMemory *pData) {
    if ((pData->flags & SET_DATA_POINTER_FLAG) == 0) {
        switch(pData->flags&DATA_MEMORY_TYPE_MASK) {
        case FLAT_DATA_MEMORY:
            if(IS_MAP(pData->map)) {
                uprv_unmapFile(pData);
            }
            break;
        case DLL_DATA_MEMORY:
            if(IS_LIBRARY(pData->lib)) {
                UNLOAD_LIBRARY(pData->lib);
            }
            break;
        default:
            break;
        }
    }
}

/* API ---------------------------------------------------------------------- */

U_CAPI UDataMemory * U_EXPORT2
udata_open(const char *path, const char *type, const char *name,
           UErrorCode *pErrorCode) {
#ifdef UDATA_DEBUG
    fprintf(stderr, "udata_open(): Opening: %s . %s\n", name, type);
    fflush(stderr);
#endif

    if(pErrorCode==NULL || U_FAILURE(*pErrorCode)) {
        return NULL;
    } else if(name==NULL || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        return NULL;
    } else {
        return doOpenChoice(path, type, name, NULL, NULL, pErrorCode);
    }
}

U_CAPI UDataMemory * U_EXPORT2
udata_openChoice(const char *path, const char *type, const char *name,
                 UDataMemoryIsAcceptable *isAcceptable, void *context,
                 UErrorCode *pErrorCode) {
#ifdef UDATA_DEBUG
  fprintf(stderr, "udata_openChoice(): Opening: %s . %s\n", name, type);fflush(stderr);
#endif

    if(pErrorCode==NULL || U_FAILURE(*pErrorCode)) {
        return NULL;
    } else if(name==NULL || *name==0 || isAcceptable==NULL) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        return NULL;
    } else {
        return doOpenChoice(path, type, name, isAcceptable, context, pErrorCode);
    }
}

U_CAPI void U_EXPORT2
udata_close(UDataMemory *pData) {
#ifdef UDATA_DEBUG
  fprintf(stderr, "udata_close()\n");fflush(stderr);
#endif

    if(pData!=NULL &&
        IS_DATA_MEMORY_LOADED(pData)) {
        // TODO:  maybe reference count cached data, rather than just
        //        permanently keeping it around?
        unloadDataMemory(pData);
        if(pData->flags & DYNAMIC_DATA_MEMORY_FLAG ) {
            uprv_free(pData);
        } else {
            pData->flags=0;
        }
    }
}

U_CAPI const void * U_EXPORT2
udata_getMemory(UDataMemory *pData) {
    if(pData!=NULL && pData->pHeader!=NULL) {
        return (char *)(pData->pHeader)+pData->pHeader->dataHeader.headerSize;
    } else {
        return NULL;
    }
}

U_CAPI void U_EXPORT2
udata_getInfo(UDataMemory *pData, UDataInfo *pInfo) {
    if(pInfo!=NULL) {
        if(pData!=NULL && pData->pHeader!=NULL) {
            const UDataInfo *info=&pData->pHeader->info;
            if(pInfo->size>info->size) {
                pInfo->size=info->size;
            }
            uprv_memcpy((uint16_t *)pInfo+1, (uint16_t *)info+1, pInfo->size-2);
        } else {
            pInfo->size=0;
        }
    }
}
