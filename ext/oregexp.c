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


static OnigEncodingType * int2encoding( VALUE v_index ) {
   int index;
   if( ! NIL_P(v_index) ) {
     index = FIX2INT(v_index);
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
#if ONIGURUMA_VERSION_MAJOR != 2      
      case 18: return ONIG_ENCODING_UTF16_BE;
      case 19: return ONIG_ENCODING_UTF16_LE;
      case 20: return ONIG_ENCODING_UTF32_BE;
      case 21: return ONIG_ENCODING_UTF32_LE;
#endif      
      case 22: return ONIG_ENCODING_EUC_JP;
      case 23: return ONIG_ENCODING_EUC_TW;
      case 24: return ONIG_ENCODING_EUC_KR;
      case 25: return ONIG_ENCODING_EUC_CN;
      case 26: return ONIG_ENCODING_SJIS;
      /*case 27: return ONIG_ENCODING_KOI8;*/
      case 28: return ONIG_ENCODING_KOI8_R;
#if ONIGURUMA_VERSION_MAJOR == 5
      case 29: return ONIG_ENCODING_CP1251;
#endif      
      case 30: return ONIG_ENCODING_BIG5;
#if ONIGURUMA_VERSION_MAJOR != 2      
      case 31: return ONIG_ENCODING_GB18030;
#endif      
      case 32: return ONIG_ENCODING_UNDEF;
   }
   }
   return ONIG_ENCODING_UNDEF;
}

static OnigSyntaxType * int2syntax( VALUE v_index ) {
   int index;
   if( ! NIL_P(v_index) ) {
     index = FIX2INT(v_index);
   switch( index ) {
#if ONIGURUMA_VERSION_MAJOR != 2      
      case 0: return ONIG_SYNTAX_ASIS;
#endif      
      case 1: return ONIG_SYNTAX_POSIX_BASIC;
      case 2: return ONIG_SYNTAX_POSIX_EXTENDED;
      case 3: return ONIG_SYNTAX_EMACS;
      case 4: return ONIG_SYNTAX_GREP;
      case 5: return ONIG_SYNTAX_GNU_REGEX;
      case 6: return ONIG_SYNTAX_JAVA;
      case 7: return ONIG_SYNTAX_PERL;
#if ONIGURUMA_VERSION_MAJOR != 2      
      case 8: return ONIG_SYNTAX_PERL_NG;
#endif      
      case 9: return ONIG_SYNTAX_RUBY;
      case 10: return ONIG_SYNTAX_DEFAULT;
   }
   }
   return ONIG_SYNTAX_DEFAULT;
}

struct callback_packet {
    VALUE hash;
    OnigRegion * region;
};

static int name_callback(
   const UChar* name, 
   const UChar* name_end,
   int ngroup_num, 
   int* group_nums,
   regex_t* reg, 
   struct callback_packet* arg
) {
   int i, gn, ref;
   OnigRegion *region = arg->region;
   VALUE nameHash = arg->hash;

   for (i = 0; i < ngroup_num; i++) {
      gn = group_nums[i];
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
   OnigEncodingType * iEncoding = int2encoding( rEncoding );
   OnigSyntaxType * iSyntax = int2syntax( rSyntax );


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

struct RMatch {
    struct RBasic basic;
    VALUE str;
    struct re_registers *regs;
};

static VALUE oregexp_make_match_data(ORegexp * oregexp, OnigRegion * region, VALUE string_str) {
    VALUE rb_cMatch = rb_const_get(rb_cObject, rb_intern("MatchData")) ;
    NEWOBJ(match, struct RMatch);
    OBJSETUP(match, rb_cMatch, T_MATCH);
    VALUE kORegexp = rb_const_get( mOniguruma, rb_intern( "ORegexp" ) ) ;
    int i , count = region->num_regs;
    struct callback_packet packet;

    match->str = rb_str_new4(string_str);
    match->regs = ALLOC(struct re_registers);
    match->regs->allocated = count;
    match->regs->num_regs = count;
    match->regs->beg = ALLOC_N(int, count);
    match->regs->end = ALLOC_N(int, count);

    for ( i = 0; i < count; i++){
        match->regs->beg[i] = region->beg[i];
        match->regs->end[i] = region->end[i];
    }
    rb_cv_set( kORegexp, "@@last_match", (VALUE)match );
    packet.region = region;
    packet.hash = rb_hash_new();
    onig_foreach_name(oregexp->reg, name_callback, &packet);
    rb_iv_set((VALUE)match, "@named_captures", packet.hash);
    return (VALUE)match;
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
      VALUE matchData = oregexp_make_match_data( oregexp, region, string_str);
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

static const UChar BACKSLASH  = 0x5c;

/* Additional backslash sequences work in substitution strings: \& (last match), \+ (last
matched group), \` (string prior to match), \' (string after match), and \\ (a literal
backslash). */

/* scan the replacement text, looking for substitutions (\n) and \escapes. */
#define MAX_GROUP_NAME_LEN   64
#if ONIGURUMA_VERSION_MAJOR == 2
#define ONIGENC_MBC_ENC_LEN(e, p)   enc_len(e, *(p))
#endif
static VALUE
oregexp_get_replacement(pat, src_text, repl_text, region)
     VALUE           pat,
     		     src_text,
                     repl_text;
     OnigRegion * region;
{
    ORegexp 	*oregexp;
    VALUE 	ret;
    int32_t  replIdx = 0, name_pos ;
    int32_t  replacementLength = RSTRING(repl_text)->len;
    UChar    *replacementText = RSTRING(repl_text)->ptr;
    UChar    *replacementEnd  = replacementText + (replacementLength-1);
    long numDigits = 0;
    long groupNum  = 0, g_start, g_end;
    OnigCodePoint digitC;
    OnigEncoding  enc;
    const UChar * matchText;
    long  matchLen;
    UChar named_group[MAX_GROUP_NAME_LEN] = {0}, *name_end;
    
    matchText = RSTRING(src_text)->ptr;
    matchLen  = RSTRING(src_text)->len;
    Data_Get_Struct( pat, ORegexp, oregexp );
    enc = onig_get_encoding( oregexp->reg );

    ret = rb_str_buf_new(RSTRING(repl_text)->len);

    while (replIdx < replacementLength) {
        OnigCodePoint c = ONIGENC_MBC_TO_CODE(enc, replacementText+replIdx, replacementEnd);
	int c_len =ONIGENC_MBC_ENC_LEN(enc, replacementText+replIdx) ; 
        if( c_len == 0 ) {
          rb_warn("Strange, for %d enc_len is 0", c);
          c_len = 1;
        }
        replIdx += c_len;
        if ( c != BACKSLASH) {
            /* Common case, no substitution, no escaping,  */
            /*  just copy the char to the dest buf. */
	    rb_str_buf_cat( ret, replacementText+replIdx-c_len, c_len);
            continue;
        }
             if (replIdx >= replacementLength) {
                rb_str_buf_cat(ret, replacementText+(replIdx-c_len), c_len);
                break;
             }
             /* Pick up a capture group number if one follows. */
            numDigits = 0;
            groupNum  = 0;
            for (;;) {
                    if (replIdx >= replacementLength) {
                            break;
                    }
		    digitC = ONIGENC_MBC_TO_CODE(enc, replacementText+replIdx, replacementEnd);
		    c_len = ONIGENC_MBC_ENC_LEN(enc, replacementText+replIdx) ; 
                    if ( ! ONIGENC_IS_CODE_DIGIT(enc, digitC) ) {
                            break;
                    }
		    replIdx += c_len;
                    groupNum=groupNum*10 + (digitC - '0');
                    numDigits++;
                    if (numDigits >= 2) { /* limit 99 groups */
                       break;
                    }
             }
             if (numDigits == 0) {
                        /* Additional backslash sequences work in substitution strings: \& (last match), \+ (last
                        matched group), \` (string prior to match), \' (string after match), and \\ (a literal
                        backslash). */
		   int p_len = c_len;
		   c = ONIGENC_MBC_TO_CODE(enc, replacementText+replIdx, replacementEnd);
		   c_len = ONIGENC_MBC_ENC_LEN(enc, replacementText+replIdx) ; 
                   switch(c) {
                        case '&' :  // matched substring
				rb_str_buf_cat(ret, matchText+region->beg[0], region->end[0] - region->beg[0]);
                                replIdx += c_len;
                                break;
                        case '`' : // prematch
                                rb_str_buf_cat(ret, matchText, region->beg[0]);
                                replIdx += c_len;
                                break;
                        case '\'': // postmatch 
				rb_str_buf_cat(ret, matchText+region->end[0], matchLen - region->end[0]);
                                replIdx += c_len;
                                break;
                        case '\\': // literal backslash
                                // place single backslash 
				rb_str_buf_cat(ret, replacementText+replIdx, c_len);
                                replIdx += c_len;
                                break;
                        case '+': // last matched group
                                replIdx += c_len;
                                for(groupNum = region->num_regs-1; groupNum > 0; groupNum --) {
                                        g_start = region->beg[ groupNum ];
                                        g_end   = region->end[ groupNum ];
                                        if( g_start != -1  ) {
					    rb_str_buf_cat(ret, matchText+g_start, g_end-g_start);
                                            break;
                                        }
                                }
                                break;
			case '<': // named group references \<name>
				name_pos = replIdx+c_len;
				name_end = named_group;
				while(name_pos < replacementLength) {
				   c = ONIGENC_MBC_TO_CODE(enc, replacementText+name_pos, replacementEnd);
				   c_len = ONIGENC_MBC_ENC_LEN(enc, replacementText+name_pos) ; 
				   name_pos += c_len;
				   if( c == '>')  break;
				   if( c < 128 && ONIGENC_IS_CODE_ALNUM(enc, c) && 
						   name_end - named_group < MAX_GROUP_NAME_LEN ) {
					*name_end = (UChar)c;
					name_end ++;
				   } else {
				        break;
				   }
				}
				if( c != '>' || name_end == named_group ) {
				   // place backslash and '<'
				   rb_str_buf_cat(ret, replacementText+(replIdx-p_len), p_len+c_len);
				   replIdx += c_len;
				} else {
				   // lookup for group and subst for that value
				   *name_end = '\0';
				   groupNum = onig_name_to_backref_number( oregexp->reg, named_group, name_end, region);
				   if( groupNum >= 0 ) {
				   	rb_str_buf_cat(ret, matchText+region->beg[groupNum], 
						   region->end[groupNum]-region->beg[groupNum]);
				   }
				   replIdx = name_pos;
				}
				break;
                        default:
                                rb_str_buf_cat(ret, replacementText+(replIdx-p_len), p_len+c_len);
                                replIdx += c_len;
                               
                   }
             } else {
                     /* Finally, append the capture group data to the destination. */
		     if( groupNum < region->num_regs && region->beg[groupNum] >= 0 ) {
	     		rb_str_buf_cat(ret, matchText+region->beg[groupNum], region->end[groupNum]-region->beg[groupNum]);
		     }
        }             
    }
    return ret;
}

static inline void
str_mod_check(s, p, len)
	VALUE s;
	char *p;
	long len;
{
	if (RSTRING(s)->ptr != p || RSTRING(s)->len != len) {
		rb_raise(rb_eRuntimeError, "string modified");
	}
}

static VALUE
oregexp_gsub(self, argc, argv,  bang, once, region)
     VALUE	     self; // pattern
     int             argc; // should be 1 if block given
     VALUE          *argv; // either replacement string 
     int             bang;
     int	     once;
     OnigRegion     *region;
{
    VALUE           repl;
    long            beg,
                    end,
                    len,
                    prev_end;
    int             tainted = 0,
		    iter = 0;
    
    VALUE buf, curr_repl,  block_res;
    ORegexp *oregexp;
    OnigEncoding enc;

    if (argc == 1 && rb_block_given_p()) {
	iter = 1;
    } else if (argc == 2) {
	repl = argv[1];
	Check_Type(repl, T_STRING);
	if (OBJ_TAINTED(argv[1]))
	    tainted = 1;
    } else {
	rb_raise(rb_eArgError, "wrong number of arguments (%d for 2)", argc);
    }
    Data_Get_Struct( self, ORegexp, oregexp );

    VALUE string_str = StringValue( argv[0] );
    UChar* str_ptr = RSTRING(string_str)->ptr;
    int str_len = RSTRING(string_str)->len;

    beg = onig_search(oregexp->reg, str_ptr, str_ptr + str_len, str_ptr, str_ptr + str_len, region, ONIG_OPTION_NONE);

    if (beg < 0) {
	/* no match */
	if (bang)
	    return Qnil;
	return rb_str_dup(string_str);
    }
    end = 0;
    buf = rb_str_buf_new(str_len);
    enc = onig_get_encoding( oregexp->reg );
    do {
	prev_end = end;
	beg = region->beg[0];
	end = region->end[0];
	rb_str_buf_cat(buf, str_ptr+prev_end, beg-prev_end);
	if ( iter ) {
	    VALUE match_data = oregexp_make_match_data( oregexp, region, string_str );
            rb_backref_set(match_data);
            block_res = rb_yield( match_data );
	    str_mod_check( string_str, str_ptr, str_len);
            curr_repl = rb_obj_as_string(block_res); 
	} else {
	    curr_repl = oregexp_get_replacement(self, string_str, repl, region);
	}
	rb_str_append(buf, curr_repl);
	if( once ) break;
	// find next match
        if( end == beg) { 
           /*
            * Always consume at least one character of the input string
            * in order to prevent infinite loops.
            */
           if( str_len <= end )
               break;
           len = ONIGENC_MBC_ENC_LEN(enc, str_ptr + end);
           rb_str_buf_cat( buf, str_ptr+end, len);
           end += len;
        }
	beg=onig_search(oregexp->reg, str_ptr, str_ptr + str_len, 
				      str_ptr+end, str_ptr + str_len, 
				      region, ONIG_OPTION_NONE);
    } while ( beg >= 0);
    rb_str_buf_cat( buf, str_ptr+end, str_len - end);

    if(tainted) 
	    OBJ_INFECT(buf, repl);
    OBJ_INFECT(buf, string_str);
    if (bang) {
	rb_funcall(string_str, rb_intern("replace"), 1, buf);
	return string_str;
    } else {
	return buf;
    }
}

typedef struct gsub_packet_t {
     VALUE	     self; // pattern
     int             argc; // should be 1 if block given
     VALUE          *argv; // either replacement string 
     int             bang;
     int	     once;
     OnigRegion     *region;
} gsub_packet;
static VALUE oregexp_packed_gsub( gsub_packet* args ) {
    return oregexp_gsub(args->self, args->argc, args->argv, args->bang, args->once, args->region);
}
void oregexp_cleanup_region(OnigRegion * region){
    onig_region_free(region, 1);       
}
static VALUE oregexp_safe_gsub(self, argc, argv,  bang, once)
     VALUE	     self; // pattern
     int             argc; // should be 1 if block given
     VALUE          *argv; // either replacement string 
     int             bang;
     int	     once;
{
        OnigRegion * region = onig_region_new();
        gsub_packet call_args = {self, argc, argv, bang, once, region};
        return rb_ensure( oregexp_packed_gsub, (VALUE)&call_args, oregexp_cleanup_region, (VALUE)region);
}

/**
 * call-seq:
 *     rxp.gsub(str, replacement)
 *     rxp.gsub(str) {|match_data| ... }
 *     
 * Returns a copy of _str_ with _all_ occurrences of _rxp_ pattern
 * replaced with either _replacement_ or the value of the block. 
 *
 * If a string is used as the replacement, the sequences \1, \2, 
 * and so on may be used to interpolate successive groups in the match.
 *
 * In the block form, the current MatchData object is passed in as a
 * parameter. The value returned by the block will be substituted for
 * the match on each call.
 *
 **/
static VALUE oregexp_m_gsub(int argc, VALUE *argv, VALUE self) {
	return oregexp_safe_gsub(self, argc, argv, 0, 0);
}

/**
 * call-seq:
 *     rxp.sub(str, replacement)
 *     rxp.sub(str) {|match_data| ... }
 *
 * Returns a copy of _str_ with the _first_ occurrence of _rxp_ pattern
 * replaced with either _replacement_ or the value of the block. 
 *
 * If a string is used as the replacement, the sequences \1, \2, 
 * and so on may be used to interpolate successive groups in the match.
 *
 * In the block form, the current MatchData object is passed in as a
 * parameter. The value returned by the block will be substituted for
 * the match on each call.
 *     
 **/
static VALUE oregexp_m_sub(int argc, VALUE *argv, VALUE self) {
	return oregexp_safe_gsub(self, argc, argv, 0, 1);
}

/**
 * call-seq:
 *     rxp.gsub!(str, replacement)
 *     rxp.gsub!(str) {|match_data| ... }
 *     
 * Performs the substitutions of ORegexp#gsub in place, returning
 * _str_, or _nil_ if no substitutions were performed.
 *
 **/
static VALUE oregexp_m_gsub_bang(int argc, VALUE *argv, VALUE self) {
	return oregexp_safe_gsub(self, argc, argv, 1, 0);
}

/**
 * call-seq:
 *     oregexp.sub!(str, replacement)
 *     oregexp.sub!(str) {|match_data| ... }
 *
 * Performs the substitutions of ORegexp#sub in place, returning
 * _str_, or _nil_ if no substitutions were performed.
 *
 **/
static VALUE oregexp_m_sub_bang(int argc, VALUE *argv, VALUE self) {
	return oregexp_safe_gsub(self, argc, argv, 1, 1);
}

static VALUE
oregexp_scan(VALUE self, VALUE str, OnigRegion * region)
{
    long            beg,
                    len,
                    end;
    int             iter = 0;
    
    VALUE           matches;
    ORegexp        *oregexp;
    OnigEncoding    enc;
    
    if ( rb_block_given_p()) {
	iter = 1;
    } 
    Data_Get_Struct( self, ORegexp, oregexp );

    VALUE string_str = StringValue( str );
    UChar* str_ptr = RSTRING(string_str)->ptr;
    int str_len = RSTRING(string_str)->len;
    beg = onig_search(oregexp->reg, str_ptr, str_ptr + str_len, str_ptr, str_ptr + str_len, region, ONIG_OPTION_NONE);
    if (beg < 0) {
	/* no match */
	return Qnil;
    }
    matches = rb_ary_new();
    enc = onig_get_encoding( oregexp -> reg );
    do {
        VALUE match_data = oregexp_make_match_data( oregexp, region, string_str );
	end = region->end[0];
	rb_ary_push( matches, match_data );
	if ( iter ) 
		rb_yield( match_data );
	// find next match
        if( end == beg) { 
           /*
            * Always consume at least one character of the input string
            * in order to prevent infinite loops.
            */
           if( str_len <= end )
               break;
           len = ONIGENC_MBC_ENC_LEN(enc, str_ptr + end);
           end += len;
        }

	beg=onig_search(oregexp->reg, str_ptr, str_ptr + str_len, 
				      str_ptr+end, str_ptr + str_len, 
				      region, ONIG_OPTION_NONE);
    } while ( beg >= 0);
    
    return matches;
}

struct scan_packet {
    VALUE self, str;
    OnigRegion * region;
};
static VALUE oregexp_packed_scan( struct scan_packet * args) {
    return oregexp_scan(args->self, args->str, args->region);
}
/**
 * call-seq:
 *     rxp.scan(str)                        # => [matchdata1, matchdata2,...] or nil
 *     rxp.scan(str) {|match_data| ... }    # => [matchdata1, matchdata2,...] or nil
 *
 * Both forms iterate through _str_, matching the pattern. For each match, 
 * a MatchData object is generated and passed to the block, and
 * added to the resulting array of MatchData objects.
 *
 * If _str_ does not match pattern, _nil_ is returned.
 *
 **/
static VALUE oregexp_m_scan(VALUE self, VALUE str) {
    OnigRegion * region = onig_region_new();
    struct scan_packet call_args = {self, str, region};
    return rb_ensure( oregexp_packed_scan, (VALUE)&call_args, oregexp_cleanup_region, (VALUE)region);
}

void Init_oregexp() {
   mOniguruma = rb_define_module("Oniguruma");
   VALUE cORegexp = rb_define_class_under(mOniguruma, "ORegexp", rb_cObject);
   rb_define_alloc_func(cORegexp, oregexp_allocate);
   rb_define_method( cORegexp, "initialize", oregexp_initialize, 2 );
   rb_define_method( cORegexp, "match", oregexp_match, 1 );
   rb_define_method( cORegexp, "gsub", oregexp_m_gsub, -1 );
   rb_define_method( cORegexp, "sub",  oregexp_m_sub,  -1 );
   rb_define_method( cORegexp, "gsub!", oregexp_m_gsub_bang, -1 );
   rb_define_method( cORegexp, "sub!",  oregexp_m_sub_bang,  -1 );
   rb_define_method( cORegexp, "scan",  oregexp_m_scan,  1 );
   rb_define_const( mOniguruma, "VERSION", rb_str_new2(onig_version()) );
}
