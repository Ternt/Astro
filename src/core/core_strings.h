// 2026-07-02

#ifndef CORE_STRINGS_H
#define CORE_STRINGS_H

/////////////////////////////
//- String8 Type

typedef struct String8 String8;
struct String8 {
  U8 *str;
  U64 size;
};

/////////////////////////////
//- String16 Type

typedef struct String16 String16;
struct String16 {
  U16 *str;
  U64 size;
};

/////////////////////////////
//- String8 Data Structure Typedef

typedef struct String8Node String8Node;
struct String8Node {
  String8Node *next;
  String8Node *prev;
  String8 v;
};

typedef struct String8List String8List;
struct String8List {
  String8List *first;
  String8List *last;
  U64 total_size;
  U64 node_count;
};

typedef struct String8Array String8Array;
struct String8Array {
  String8 *v;
  U64 total_size;
  U64 count;
};

/////////////////////////////
//- Cstring Length Helpers

static U64 Cstr8_Length(U8 *cstr);
static U64 Cstr16_Length(U16 *cstr);

/////////////////////////////
//- String8 Constructors

static String8 Str8_Make(U8 *str, U64 size);
static String8 Str8_Zero(void);
static String8 Str8_Cstring(U8 *cstr);
#define Str8_Lit(cstr) Str8_Make(cstr,sizeof(cstr)-1)
#define Str8_CompLit(cstr) (String8){cstr,sizeof(cstr)-1}

/////////////////////////////
//- String16 Constructors

static String16 Str16_Make(U16 *str, U64 size);
static String16 Str16_Zero(void);
static String16 Str16_Cstring(U16 *cstr);
#define Str16_Lit(cstr) Str16_Make(cstr,sizeof(cstr)-1)
#define Str16_CompLit(cstr) (String16){cstr,sizeof(cstr)-1}

#endif // CORE_STRINGS_H
