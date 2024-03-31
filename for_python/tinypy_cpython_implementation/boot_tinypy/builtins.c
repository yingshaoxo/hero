/* File: Builtins
 * Builtin tinypy functions.
 */

/*
 *typedef union tp_obj {
    int type;
    tp_number_ number;
    struct { int type; int *data; } gci;
    tp_string_ string;
    tp_dict_ dict;
    tp_list_ list;
    tp_fnc_ fnc;
    tp_data_ data;
} tp_obj;

typedef struct tp_number_ {
    int type;
    tp_num val;
} tp_number_;
typedef struct tp_string_ {
    int type;
    struct _tp_string *info;
    char const *val;
    int len;
} tp_string_;
typedef struct tp_list_ {
    int type;
    struct _tp_list *val;
} tp_list_;
typedef struct tp_dict_ {
    int type;
    struct _tp_dict *val;
    int dtype;
} tp_dict_;
typedef struct tp_fnc_ {
    int type;
    struct _tp_fnc *info;
    int ftype;
    void *cfnc;
} tp_fnc_;
typedef struct tp_data_ {
    int type;
    struct _tp_data *info;
    void *val;
    int magic;
} tp_data_;
 */

#include <stdbool.h>
#include <unistd.h>

/* tinypy API to be use in this unit */
extern tp_obj tp_data(TP,int magic,void *v);
extern tp_obj tp_object_new(TP);
extern tp_obj tp_object(TP);
extern tp_obj tp_method(TP,tp_obj self,tp_obj v(TP));
extern tp_obj tp_string_copy(TP, const char *s, int n);
extern tp_obj tp_list(TP);
extern tp_obj tp_copy(TP);

tp_obj tp_input_(TP) {
	tp_obj argument1 = TP_OBJ();
    printf("%s", argument1.string.val);
    fflush(stdout);

    char *the_input = malloc(sizeof(char) * 5001);
    scanf("%s", the_input);

    return tp_string(the_input);
}

tp_obj tp_sleep_(TP) {
	tp_obj argument1 = TP_OBJ();

    sleep(argument1.number.val);

    return tp_None;
}

/* run_command */
/* run */
bool _ypython_is_general_space(char c)
{
    switch (c)
    {
    case ' ':
    case '\n':
    case '\t':
    case '\f':
    case '\r':
        return true;
        break;
    default:
        return false;
        break;
    }
}

char *_ypython_string_left_strip(char *s)
{
    while (_ypython_is_general_space(*s))
    {
        s++;
    }
    return s;
}

char *_ypython_string_right_strip(char *s)
{
    char *back = s + strlen(s) - 1;
    while (_ypython_is_general_space(*back))
    {
        --back;
    }
    *(back + 1) = '\0';
    return s;
}

/*
strip \s before and after a string.
*/
const char *ypython_string_strip(char *s)
{
    return (const char *)_ypython_string_right_strip(_ypython_string_left_strip(s));
}

char *_ypython_get_infinate_length_text_line(FILE *f)
{
    size_t size = 0;
    size_t len = 0;
    size_t last = 0;
    char *buf = NULL;

    do
    {
        size += BUFSIZ;                                              /* BUFSIZ is defined as "the optimal read size for this platform" */
        buf = (char *) realloc(buf, size); /* realloc(NULL,n) is the same as malloc(n) */
        /* Actually do the read. Note that fgets puts a terminal '\0' on the
           end of the string, so we make sure we overwrite this */
        if (buf == NULL)
            return NULL;
        fgets(buf + last, BUFSIZ, f);
        len = strlen(buf);
        last = len - 1;
    } while (!feof(f) && buf[last] != '\n');

    return buf;
}

char *_ypython_get_infinate_length_text(FILE *f)
{
    size_t size = 0;
    size_t len = 0;
    size_t last = 0;
    char *buf = NULL;

    do
    {
        size += BUFSIZ;                                              /* BUFSIZ is defined as "the optimal read size for this platform" */
        buf = (char *)realloc(buf, size); /* realloc(NULL,n) is the same as malloc(n) */
        /* Actually do the read. Note that fgets puts a terminal '\0' on the
           end of the string, so we make sure we overwrite this */
        if (buf == NULL)
            return NULL;
        fgets(buf + last, BUFSIZ, f);
        len = strlen(buf);
        last = len - 1;

        if (buf[last] == '\n')
        {
            last = len;
        }
    } while (!feof(f));

    return buf;
}

/*
Run a bash command and return the result as a string.
*/
char *ypython_run_command(const char *bash_command_line)
{
    FILE *FileOpen;
    FileOpen = popen(bash_command_line, "r");

    char *all_lines = _ypython_get_infinate_length_text(FileOpen);
    pclose(FileOpen);

    return (char *)ypython_string_strip(all_lines);
}

/*
Run a bash command and wait for it to get finished, it won't return anything.
*/
void ypython_run(const char *bash_command_line)
{
    FILE *FileOpen;
    FileOpen = popen(bash_command_line, "r");

    while (!feof(FileOpen))
    {
        char *a_line = _ypython_get_infinate_length_text_line(FileOpen);
        printf("%s", a_line);
    }

    pclose(FileOpen);
}

tp_obj tp_run_command(TP) {
	tp_obj argument1 = TP_OBJ();

    /*
    tp_obj argument;
    int length = tp->params.list.val->len;
    int index; 
    for (index=0; index<length; index++) {
        argument = _tp_list_get(tp,tp->params.list.val,index,"TP_LOOP");
        //printf("%s", argument.string.val);
        break;
    }
    */

    return tp_string(ypython_run_command(argument1.string.val));
}

tp_obj tp_run_(TP) {
	tp_obj argument1 = TP_OBJ();

    ypython_run(argument1.string.val);

    return tp_None;
}

/* open */
static tp_obj _tp_open_read(TP) {
	tp_obj the_self = TP_OBJ();

	tp_obj a_data = tp_get(tp, the_self, tp_string("__file_pointer__"));
    FILE *a_file = a_data.data.val;

    tp_obj the_text = tp_string("");

    char buffer[1024];
    size_t bytes_read;
    while (true) {
        bytes_read = fread(buffer, 1, 1024, a_file);

        if (bytes_read <= 0) {
            break;
        }

        the_text = tp_add(tp, the_text, tp_string(buffer));
    }

    return the_text;
}

static tp_obj _tp_open_write(TP) {
	tp_obj the_self = TP_OBJ();

	tp_obj a_data = tp_get(tp, the_self, tp_string("__file_pointer__"));
    FILE *a_file = a_data.data.val;

    tp_obj argument;
    int length = tp->params.list.val->len;
    int index; 
    for (index=0; index<length; index++) {
        argument = _tp_list_get(tp,tp->params.list.val,index,"TP_LOOP");
        //printf("%s", argument.string.val);
        break;
    }

    fwrite(argument.string.val, sizeof(char), strlen(argument.string.val), a_file);

    return tp_None;
}

static tp_obj _tp_open_close(TP) {
	tp_obj the_self = TP_OBJ();

	tp_obj a_data = tp_get(tp, the_self, tp_string("__file_pointer__"));
    FILE *a_file = a_data.data.val;
    fclose(a_file);

    return tp_None;
}

tp_obj tp_open(TP) {
    const char *file_path;
    const char *handle_mode;

    int n = 0;
    tp_obj e;
    TP_LOOP(e)
        if (n == 0) {
            file_path = e.string.val;
        } else if (n == 1) {
            handle_mode = e.string.val;
        }
        n += 1;
    TP_END;

    //printf("%s\n%s\n", file_path, handle_mode);

	tp_obj the_return_object = tp_object(tp);	
	tp_set(tp, the_return_object, tp_string("read"), tp_method(tp, the_return_object, _tp_open_read));
	tp_set(tp, the_return_object, tp_string("write"), tp_method(tp, the_return_object, _tp_open_write));
	tp_set(tp, the_return_object, tp_string("close"), tp_method(tp, the_return_object, _tp_open_close));

	tp_obj data_in_object;				
    FILE *a_file = fopen(file_path, handle_mode);
	data_in_object = tp_data(tp, (int)(sizeof(FILE*)), a_file);
	tp_set(tp, the_return_object, tp_string("__file_pointer__"), data_in_object);

	tp_set(tp, the_return_object, tp_string("_data"), tp_string("yingshaoxo"));

    //return tp_None;
    return the_return_object;
}

/* print */
tp_obj tp_print(TP) {
    int n = 0;
    tp_obj e;
    TP_LOOP(e)
        if (n) { printf(" "); }
        tp_echo(tp,e);
        n += 1;
    TP_END;
    printf("\n");
    return tp_None;
}

tp_obj tp_bind(TP) {
    tp_obj r = TP_TYPE(TP_FNC);
    tp_obj self = TP_OBJ();
    return tp_fnc_new(tp,
        r.fnc.ftype|2,r.fnc.cfnc,r.fnc.info->code,
        self,r.fnc.info->globals);
}

tp_obj tp_min(TP) {
    tp_obj r = TP_OBJ();
    tp_obj e;
    TP_LOOP(e)
        if (tp_cmp(tp,r,e) > 0) { r = e; }
    TP_END;
    return r;
}

tp_obj tp_max(TP) {
    tp_obj r = TP_OBJ();
    tp_obj e;
    TP_LOOP(e)
        if (tp_cmp(tp,r,e) < 0) { r = e; }
    TP_END;
    return r;
}

tp_obj tp_copy(TP) {
    tp_obj r = TP_OBJ();
    int type = r.type;
    if (type == TP_LIST) {
        return _tp_list_copy(tp,r);
    } else if (type == TP_DICT) {
        return _tp_dict_copy(tp,r);
    }
    tp_raise(tp_None,tp_string("(tp_copy) TypeError: ?"));
}


tp_obj tp_len_(TP) {
    tp_obj e = TP_OBJ();
    return tp_len(tp,e);
}

tp_obj tp_assert(TP) {
    int a = TP_NUM();
    if (a) { return tp_None; }
    tp_raise(tp_None,tp_string("(tp_assert) AssertionError"));
}

tp_obj tp_range(TP) {
    int a,b,c,i;
    tp_obj r = tp_list(tp);
    switch (tp->params.list.val->len) {
        case 1: a = 0; b = TP_NUM(); c = 1; break;
        case 2:
        case 3: a = TP_NUM(); b = TP_NUM(); c = TP_DEFAULT(tp_number(1)).number.val; break;
        default: return r;
    }
    if (c != 0) {
        for (i=a; (c>0) ? i<b : i>b; i+=c) {
            _tp_list_append(tp,r.list.val,tp_number(i));
        }
    }
    return r;
}

/* Function: tp_system
 *
 * The system builtin. A grave security flaw. If your version of tinypy
 * enables this, you better remove it before deploying your app :P
 */
tp_obj tp_system(TP) {
    char s[TP_CSTR_LEN]; tp_cstr(tp,TP_STR(),s,TP_CSTR_LEN);
    int r = system(s);
    return tp_number(r);
}

tp_obj tp_istype(TP) {
    tp_obj v = TP_OBJ();
    tp_obj t = TP_STR();
    if (tp_cmp(tp,t,tp_string("string")) == 0) { return tp_number(v.type == TP_STRING); }
    if (tp_cmp(tp,t,tp_string("list")) == 0) { return tp_number(v.type == TP_LIST); }
    if (tp_cmp(tp,t,tp_string("dict")) == 0) { return tp_number(v.type == TP_DICT); }
    if (tp_cmp(tp,t,tp_string("number")) == 0) { return tp_number(v.type == TP_NUMBER); }
    if (tp_cmp(tp,t,tp_string("fnc")) == 0) { return tp_number(v.type == TP_FNC && (v.fnc.ftype&2) == 0); }
    if (tp_cmp(tp,t,tp_string("method")) == 0) { return tp_number(v.type == TP_FNC && (v.fnc.ftype&2) != 0); }
    tp_raise(tp_None,tp_string("(is_type) TypeError: ?"));
}


tp_obj tp_float(TP) {
    tp_obj v = TP_OBJ();
    int ord = TP_DEFAULT(tp_number(0)).number.val;
    int type = v.type;
    if (type == TP_NUMBER) { return v; }
    if (type == TP_STRING && v.string.len < 32) {
        char s[32]; memset(s,0,v.string.len+1);
        memcpy(s,v.string.val,v.string.len);
        if (strchr(s,'.')) { return tp_number(atof(s)); }
        return(tp_number(strtol(s,0,ord)));
    }
    tp_raise(tp_None,tp_string("(tp_float) TypeError: ?"));
}


tp_obj tp_save(TP) {
    char fname[256]; tp_cstr(tp,TP_STR(),fname,256);
    tp_obj v = TP_OBJ();
    FILE *f;
    f = fopen(fname,"wb");
    if (!f) { tp_raise(tp_None,tp_string("(tp_save) IOError: ?")); }
    fwrite(v.string.val,v.string.len,1,f);
    fclose(f);
    return tp_None;
}

tp_obj tp_load(TP) {
    FILE *f;
    long l;
    tp_obj r;
    char *s;
    char fname[256]; tp_cstr(tp,TP_STR(),fname,256);
    struct stat stbuf;
    stat(fname, &stbuf);
    l = stbuf.st_size;
    f = fopen(fname,"rb");
    if (!f) {
        tp_raise(tp_None,tp_string("(tp_load) IOError: ?"));
    }
    r = tp_string_t(tp,l);
    s = r.string.info->s;
    fread(s,1,l,f);
/*    if (rr !=l) { printf("hmmn: %d %d\n",rr,(int)l); }*/
    fclose(f);
    return tp_track(tp,r);
}


tp_obj tp_fpack(TP) {
    tp_num v = TP_NUM();
    tp_obj r = tp_string_t(tp,sizeof(tp_num));
    *(tp_num*)r.string.val = v;
    return tp_track(tp,r);
}

tp_obj tp_abs(TP) {
    return tp_number(fabs(tp_float(tp).number.val));
}
tp_obj tp_int(TP) {
    return tp_number((long)tp_float(tp).number.val);
}
tp_num _roundf(tp_num v) {
    tp_num av = fabs(v); tp_num iv = (long)av;
    av = (av-iv < 0.5?iv:iv+1);
    return (v<0?-av:av);
}
tp_obj tp_round(TP) {
    return tp_number(_roundf(tp_float(tp).number.val));
}

tp_obj tp_exists(TP) {
    char fname[TP_CSTR_LEN]; tp_cstr(tp,TP_STR(),fname,TP_CSTR_LEN);
    struct stat stbuf;
    return tp_number(!stat(fname,&stbuf));
}
tp_obj tp_mtime(TP) {
    char fname[TP_CSTR_LEN]; tp_cstr(tp,TP_STR(),fname,TP_CSTR_LEN);
    struct stat stbuf;
    if (!stat(fname,&stbuf)) { return tp_number(stbuf.st_mtime); }
    tp_raise(tp_None,tp_string("(tp_mtime) IOError: ?"));
}

int _tp_lookup_(TP,tp_obj self, tp_obj k, tp_obj *meta, int depth) {
    int n = _tp_dict_find(tp,self.dict.val,k);
    if (n != -1) {
        *meta = self.dict.val->items[n].val;
        return 1;
    }
    depth--; if (!depth) { tp_raise(0,tp_string("(tp_lookup) RuntimeError: maximum lookup depth exceeded")); }
    if (self.dict.dtype && self.dict.val->meta.type == TP_DICT && _tp_lookup_(tp,self.dict.val->meta,k,meta,depth)) {
        if (self.dict.dtype == 2 && meta->type == TP_FNC) {
            *meta = tp_fnc_new(tp,meta->fnc.ftype|2,
                meta->fnc.cfnc,meta->fnc.info->code,
                self,meta->fnc.info->globals);
        }
        return 1;
    }
    return 0;
}

int _tp_lookup(TP,tp_obj self, tp_obj k, tp_obj *meta) {
    return _tp_lookup_(tp,self,k,meta,8);
}

#define TP_META_BEGIN(self,name) \
    if (self.dict.dtype == 2) { \
        tp_obj meta; if (_tp_lookup(tp,self,tp_string(name),&meta)) {

#define TP_META_END \
        } \
    }

/* Function: tp_setmeta
 * Set a "dict's meta".
 *
 * This is a builtin function, so you need to use <tp_params> to provide the
 * parameters.
 *
 * In tinypy, each dictionary can have a so-called "meta" dictionary attached
 * to it. When dictionary attributes are accessed, but not present in the
 * dictionary, they instead are looked up in the meta dictionary. To get the
 * raw dictionary, you can use <tp_getraw>.
 *
 * This function is particulary useful for objects and classes, which are just
 * special dictionaries created with <tp_object> and <tp_class>. There you can
 * use tp_setmeta to change the class of the object or parent class of a class.
 *
 * Parameters:
 * self - The dictionary for which to set a meta.
 * meta - The meta dictionary.
 *
 * Returns:
 * None
 */
tp_obj tp_setmeta(TP) {
    tp_obj self = TP_TYPE(TP_DICT);
    tp_obj meta = TP_TYPE(TP_DICT);
    self.dict.val->meta = meta;
    return tp_None;
}

tp_obj tp_getmeta(TP) {
    tp_obj self = TP_TYPE(TP_DICT);
    return self.dict.val->meta;
}

/* Function: tp_object
 * Creates a new object.
 *
 * Returns:
 * The newly created object. The object initially has no parent class, use
 * <tp_setmeta> to set a class. Also see <tp_object_new>.
 */
tp_obj tp_object(TP) {
    tp_obj self = tp_dict(tp);
    self.dict.dtype = 2;
    return self;
}

tp_obj tp_object_new(TP) {
    tp_obj klass = TP_TYPE(TP_DICT);
    tp_obj self = tp_object(tp);
    self.dict.val->meta = klass;
    TP_META_BEGIN(self,"__init__");
        tp_call(tp,meta,tp->params);
    TP_META_END;
    return self;
}

tp_obj tp_object_call(TP) {
    tp_obj self;
    if (tp->params.list.val->len) {
        self = TP_TYPE(TP_DICT);
        self.dict.dtype = 2;
    } else {
        self = tp_object(tp);
    }
    return self;
}

/* Function: tp_getraw
 * Retrieve the raw dict of a dict.
 *
 * This builtin retrieves one dict parameter from tinypy, and returns its raw
 * dict. This is very useful when implementing your own __get__ and __set__
 * functions, as it allows you to directly access the attributes stored in the
 * dict.
 */
tp_obj tp_getraw(TP) {
    tp_obj self = TP_TYPE(TP_DICT);
    self.dict.dtype = 0;
    return self;
}

/* Function: tp_class
 * Creates a new base class.
 *
 * Parameters:
 * none
 *
 * Returns:
 * A new, empty class (derived from tinypy's builtin "object" class).
 */
tp_obj tp_class(TP) {
    tp_obj klass = tp_dict(tp);
    klass.dict.val->meta = tp_get(tp,tp->builtins,tp_string("object")); 
    return klass;
}

/* Function: tp_builtins_bool
 * Coerces any value to a boolean.
 */
tp_obj tp_builtins_bool(TP) {
    tp_obj v = TP_OBJ();
    return (tp_number(tp_bool(tp, v)));
}
