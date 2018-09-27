#include "ruby.h"
#include "extconf.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "ahocorasick.h"

void callback_match_pos(VALUE rb_result_container, void *arg, struct aho_match_t* m)
{
    
    unsigned int i = m->pos, idx = 0;

    VALUE key = LONG2NUM(m->id);
    VALUE hash_value = rb_hash_aref(rb_result_container, key);

    if (NIL_P(hash_value)) {
      hash_value = rb_ary_new();
      rb_hash_aset(rb_result_container, key, hash_value);
    }
    rb_ary_push(hash_value, LONG2NUM(m->pos)) ;
}

VALUE multi_string_match(VALUE self, VALUE body, VALUE keys)
{
  int state;
  VALUE result = rb_hash_new();
  rb_eval_string("puts \"start1\" ");
  struct ahocorasick aho;
  aho_init(&aho);
  char *target = StringValueCStr(body);
  long size =  RARRAY_LEN(keys);
  
  for(long idx = 0; idx < size; idx++) {
    VALUE entry = rb_ary_entry(keys, idx);
    char *key_text = StringValueCStr(entry);
    aho_add_match_text(&aho,  key_text, strlen(key_text));
  }
  aho_create_trie(&aho);
  aho_register_match_callback(result, &aho, callback_match_pos, (void*)target);
  long count = aho_findtext(&aho, target, strlen(target));
  aho_destroy(&aho);
  return result;
}

void Init_multi_string_replace()
{
  VALUE mod = rb_define_module("MultiStringReplaceExt");
  rb_define_singleton_method(mod, "match", multi_string_match, 2);
}

