#include <ruby.h>
#include <oniguruma.h>
/*
   TODO:
      - Add named backreferences.
*/

typedef struct _oregexp {
   regex_t * reg;
} ORegexp;

VALUE mOniguruma;
VALUE nameHash;

static void oregexp_free( ORegexp  * oregexp) {
   onig_free( oregexp->reg );
   free( oregexp );
}

static VALUE oregexp_allocate( VALUE klass ) {
   ORegexp * oregexp = malloc( sizeof( ORegexp ) );
   oregexp->reg = NULL;
   return Data_Wrap_Struct( klass, 0, oregexp_free, oregexp );
}


static OnigEncodingType * int2encoding( int index ) {
   switch( index ) {
      case 0: return ONIG_ENCODING_ASCII;
      case 1: return ONIG_ENCODING_ISO_8859_1;
      case 2: return ONIG_ENCODING_ISO_8859_2;
      case 3: return ONIG_ENCODING_ISO_8859_3;
      case 4: return ONIG_ENCODING_ISO_8859_4;
      case 5: return ONIG_ENCODING_ISO_8859_5;
      case 6: return ONIG_ENCODING_ISO_8859_6;
      case 7: return ONIG_ENCODING_ISO_8859_7;
      case 8: return ONIG_ENCODING_ISO_8859_8;
      case 9: return ONIG_ENCODING_ISO_8859_9;
      case 10: return ONIG_ENCODING_ISO_8859_10;
      case 11: return ONIG_ENCODING_ISO_8859_11;
      case 12: return ONIG_ENCODING_ISO_8859_11;
      case 13: return ONIG_ENCODING_ISO_8859_13;
      case 14: return ONIG_ENCODING_ISO_8859_14;
      case 15: return ONIG_ENCODING_ISO_8859_15;
      case 16: return ONIG_ENCODING_ISO_8859_16;
      case 17: return ONIG_ENCODING_UTF8;
      case 18: return ONIG_ENCODING_UTF16_BE;
      case 19: return ONIG_ENCODING_UTF16_LE;
      case 20: return ONIG_ENCODING_UTF32_BE;
      case 21: return ONIG_ENCODING_UTF32_LE;
      case 22: return ONIG_ENCODING_EUC_JP;
      case 23: return ONIG_ENCODING_EUC_TW;
      case 24: return ONIG_ENCODING_EUC_KR;
      case 25: return ONIG_ENCODING_EUC_CN;
      case 26: return ONIG_ENCODING_SJIS;
      /*case 27: return ONIG_ENCODING_KOI8;*/
      case 28: return ONIG_ENCODING_KOI8_R;
      case 29: return ONIG_ENCODING_CP1251;
      case 30: return ONIG_ENCODING_BIG5;
      case 31: return ONIG_ENCODING_GB18030;
      case 32: return ONIG_ENCODING_UNDEF;
   }
   return ONIG_ENCODING_UNDEF;
}

static OnigSyntaxType * int2syntax( int index ) {
   switch( index ) {
      case 0: return ONIG_SYNTAX_ASIS;
      case 1: return ONIG_SYNTAX_POSIX_BASIC;
      case 2: return ONIG_SYNTAX_POSIX_EXTENDED;
      case 3: return ONIG_SYNTAX_EMACS;
      case 4: return ONIG_SYNTAX_GREP;
      case 5: return ONIG_SYNTAX_GNU_REGEX;
      case 6: return ONIG_SYNTAX_JAVA;
      case 7: return ONIG_SYNTAX_PERL;
      case 8: return ONIG_SYNTAX_PERL_NG;
      case 9: return ONIG_SYNTAX_RUBY;
      case 10: return ONIG_SYNTAX_DEFAULT;
   }
   return ONIG_SYNTAX_DEFAULT;
}

static int name_callback(
   const UChar* name, 
   const UChar* name_end,
   int ngroup_num, 
   int* group_nums,
   regex_t* reg, 
   void* arg
) {
   int i, gn, ref;
   OnigRegion *region = (OnigRegion* )arg;

   for (i = 0; i < ngroup_num; i++) {
      gn = group_nums[i];
      ref = onig_name_to_backref_number(reg, name, name_end, region);
      if (ref != gn )
         rb_raise(rb_eException, "Oniguruma Error: group and backreference names are different");
      rb_hash_aset( nameHash, ID2SYM(rb_intern(name)), INT2FIX( gn ) );
  }
  return 0;  
}

static VALUE oregexp_initialize( VALUE self, VALUE pattern, VALUE options ) { 
   ORegexp *oregexp;
   Data_Get_Struct( self, ORegexp, oregexp );

   VALUE pattern_str = StringValue( pattern );
   rb_iv_set( self, "@pattern", pattern_str );
   rb_iv_set( self, "@options", options );
   UChar* pat_ptr = RSTRING(pattern_str)->ptr;
   int pat_len = RSTRING(pattern_str)->len;

   VALUE rOptions = rb_hash_aref( options, ID2SYM( rb_intern( "options" ) ) );
   VALUE rEncoding = rb_hash_aref( options, ID2SYM( rb_intern( "encoding" ) ) );
   VALUE rSyntax = rb_hash_aref( options, ID2SYM( rb_intern( "syntax" ) ) );
   int iOptions = NUM2INT( rOptions );
   int iEncoding = int2encoding( rEncoding );
   int iSyntax = int2syntax( rSyntax );


   int r;
   OnigErrorInfo einfo;
   r = onig_new(&(oregexp->reg), pat_ptr, pat_ptr + pat_len, iOptions, iEncoding, iSyntax, &einfo);
   if (r != ONIG_NORMAL) {
      char s[ONIG_MAX_ERROR_MESSAGE_LEN];
      onig_error_code_to_str(s, r, &einfo);
      rb_raise(rb_eException, "Oniguruma Error: %s", s);
   }
   return self;
}

/*
 *  call-seq:
 *     rxp.match(str)   => matchdata or nil
 *  
 *  Returns a <code>MatchData</code> object describing the match, or
 *  <code>nil</code> if there was no match. This is equivalent to retrieving the
 *  value of the special variable <code>$~</code> following a normal match.
 *     
 *     /(.)(.)(.)/.match("abc")[2]   #=> "b"
 */
static VALUE oregexp_match( VALUE self, VALUE string ) {
   ORegexp *oregexp;
   Data_Get_Struct( self, ORegexp, oregexp );

   VALUE string_str = StringValue( string );
   UChar* str_ptr = RSTRING(string_str)->ptr;
   int str_len = RSTRING(string_str)->len;

   OnigRegion *region = onig_region_new();
   int r = onig_search(oregexp->reg, str_ptr, str_ptr + str_len, str_ptr, str_ptr + str_len, region, ONIG_OPTION_NONE);
   if (r >= 0) {
   
      VALUE begins = rb_ary_new();
      VALUE ends = rb_ary_new();
      nameHash = rb_hash_new();
      
      onig_foreach_name(oregexp->reg, name_callback, (void* )region);

 
      int i;

      for (i = 0; i < region->num_regs; i++) {
         rb_ary_push( begins, INT2FIX( region->beg[i] ) );
         rb_ary_push( ends, INT2FIX( region->end[i] ) );
      }
      VALUE kMatchData = rb_const_get( mOniguruma, rb_intern( "MatchData" ) );
      VALUE kORegexp = rb_const_get( mOniguruma, rb_intern( "ORegexp" ) );
      VALUE matchData = rb_funcall(kMatchData, rb_intern("new"), 4, string_str, begins, ends, nameHash ); 
      rb_cv_set( kORegexp, "@@last_match", matchData );

      onig_region_free(region, 1 );
      return matchData;
   } else if (r == ONIG_MISMATCH) {
      onig_region_free(region, 1 );
      return Qnil;
   } else {
      onig_region_free(region, 1 );
      char s[ONIG_MAX_ERROR_MESSAGE_LEN];
      onig_error_code_to_str(s, r);
      rb_raise(rb_eException, "Oniguruma Error: %s", s);
   }

}

void Init_oregexp() {
   mOniguruma = rb_define_module("Oniguruma");
   VALUE cORegexp = rb_define_class_under(mOniguruma, "ORegexp", rb_cObject);
   rb_define_alloc_func(cORegexp, oregexp_allocate);
   rb_define_method( cORegexp, "initialize", oregexp_initialize, 2 );
   rb_define_method( cORegexp, "match", oregexp_match, 1 );
   
}
