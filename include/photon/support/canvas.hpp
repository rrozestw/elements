/*=================================================================================================   Copyright (c) 2016 Joel de Guzman   Licensed under a Creative Commons Attribution-ShareAlike 4.0 International.   http://creativecommons.org/licenses/by-sa/4.0/=================================================================================================*/#if !defined(PHOTON_GUI_LIB_CANVAS_MAY_3_2016)#define PHOTON_GUI_LIB_CANVAS_MAY_3_2016#include <photon/support/color.hpp>#include <photon/support/rect.hpp>#include <photon/support/circle.hpp>#include <photon/support/pixmap.hpp>#include <vector>#include <functional>#include <stack>#include <cmath>#include <cassert>#include <cairo.h>namespace photon{   class canvas   {   public:                        canvas(cairo_t& context_)                         : _context(context_)                        {}                        canvas(canvas const& rhs) = delete;      canvas&           operator=(canvas const& rhs) = delete;      cairo_t&          cairo_context() const;      /////////////////////////////////////////////////////////////////////////////////////////////      // Transforms      void              translate(point p);      void              rotate(float rad);      void              scale(point p);      point             device_to_user(point p);      point             user_to_device(point p);      /////////////////////////////////////////////////////////////////////////////////////////////      // Paths      void              begin_path();      void              close_path();      void              fill();      void              stroke();      void              clip();      bool              hit_test(point p);      void              move_to(point p);      void              line_to(point p);      void              arc_to(point p1, point p2, float radius);      void              arc(                           point p, float radius,                           float start_angle, float end_angle,                           bool ccw = false                        );      void              rect(struct rect r);      void              round_rect(struct rect r, float radius);      void              circle(struct circle c);      /////////////////////////////////////////////////////////////////////////////////////////////      // Styles      void              fill_style(color c);      void              stroke_style(color c);      void              line_width(float w);      void              shadow_style(point offs, float blur, color c = colors::black);      /////////////////////////////////////////////////////////////////////////////////////////////      // Gradients      struct color_stop      {         float offset;         color color;      };      struct linear_gradient      {         point start;         point end;         void  add_color_stop(color_stop cs);         std::vector<color_stop> space;      };      struct radial_gradient      {         point start;         float start_radius;         point end;         float end_radius;         void  add_color_stop(color_stop cs);         std::vector<color_stop> space;      };      void              fill_style(linear_gradient const& gr);      void              fill_style(radial_gradient const& gr);      /////////////////////////////////////////////////////////////////////////////////////////////      // Rectangles      void              fill_rect(struct rect r);      void              fill_round_rect(struct rect r, float radius);      void              stroke_rect(struct rect r);      void              stroke_round_rect(struct rect r, float radius);      /////////////////////////////////////////////////////////////////////////////////////////////      // Font      enum font_style      {         normal   = 0,         bold     = 1,         italic   = 2      };      void              font(char const* face, float size = 16, int style = normal);      /////////////////////////////////////////////////////////////////////////////////////////////      // Text      enum text_alignment      {         // Horizontal align         left     = 0,        // Default, align text horizontally to left.         center   = 1,        // Align text horizontally to center.         right    = 2,        // Align text horizontally to right.         // Vertical align         baseline = 4,        // Default, align text vertically to baseline.         top      = 8,        // Align text vertically to top.         middle   = 12,       // Align text vertically to middle.         bottom   = 16        // Align text vertically to bottom.      };      struct text_metrics      {         float       ascent;         float       descent;         float       leading;         point       size;      };      void              fill_text(point p, char const* utf8);      void              stroke_text(point p, char const* utf8);      text_metrics      measure_text(char const* utf8);      void              text_align(int align);      /////////////////////////////////////////////////////////////////////////////////////////////      // Pixmaps      void              draw(pixmap const& pm, photon::rect src, photon::rect dest);      void              draw(pixmap const& pm, photon::rect dest);      void              draw(pixmap const& pm, point pos);      /////////////////////////////////////////////////////////////////////////////////////////////      // States      class state      {      public:                        state(canvas& cnv_);                        state(state&& rhs);                        state(state const&) = delete;                        ~state();         state&         operator=(state const&) = delete;         state&         operator=(state&& rhs);      private:         canvas* cnv;      };      state             new_state()   { return state{ *this }; }      void              save();      void              restore();   private:      struct canvas_state      {         std::function<void()>   stroke_style   = [](){};         std::function<void()>   fill_style     = [](){};         int                     align          = 0;      };      using state_stack = std::stack<canvas_state>;      cairo_t&          _context;      canvas_state      _state;      state_stack       _state_stack;   };   ////////////////////////////////////////////////////////////////////////////////////////////////   // Inlines   ////////////////////////////////////////////////////////////////////////////////////////////////   inline cairo_t& canvas::cairo_context() const   {      return _context;   }   inline void canvas::translate(point p)   {      cairo_translate(&_context, p.x, p.y);   }   inline void canvas::rotate(float rad)   {      cairo_rotate(&_context, rad);   }   inline void canvas::scale(point p)   {      cairo_scale(&_context, p.x, p.y);   }   inline point canvas::device_to_user(point p)   {      double x = p.x;      double y = p.y;      cairo_device_to_user(&_context, &x, &y);      return { float(x), float(y) };   }   inline point canvas::user_to_device(point p)   {      double x = p.x;      double y = p.y;      cairo_user_to_device(&_context, &x, &y);      return { float(x), float(y) };   }   inline void canvas::begin_path()   {      cairo_new_path(&_context);   }   inline void canvas::close_path()   {      cairo_close_path(&_context);   }   inline void canvas::fill()   {      auto save_src = cairo_pattern_reference(cairo_get_source(&_context));      _state.fill_style();      cairo_fill(&_context);      cairo_set_source(&_context, save_src);      cairo_pattern_destroy(save_src);   }   inline void canvas::stroke()   {      auto save_src = cairo_pattern_reference(cairo_get_source(&_context));      _state.stroke_style();      cairo_stroke(&_context);      cairo_set_source(&_context, save_src);      cairo_pattern_destroy(save_src);   }   inline void canvas::clip()   {      cairo_clip(&_context);   }   inline bool canvas::hit_test(point p)   {      return cairo_in_fill(&_context, p.x, p.y);   }   inline void canvas::move_to(point p)   {      cairo_move_to(&_context, p.x, p.y);   }   inline void canvas::line_to(point p)   {      cairo_line_to(&_context, p.x, p.y);   }   inline void canvas::arc_to(point p1, point p2, float radius)   {      assert(false); // unimplemented   }   inline void canvas::arc(      point p, float radius,      float start_angle, float end_angle,      bool ccw   )   {      if (ccw)         cairo_arc_negative(&_context, p.x, p.y, radius, start_angle, end_angle);      else         cairo_arc(&_context, p.x, p.y, radius, start_angle, end_angle);   }   inline void canvas::rect(struct rect r)   {      cairo_rectangle(&_context, r.left, r.top, r.width(), r.height());   }   inline void canvas::round_rect(struct rect bounds, float radius)   {      auto x = bounds.left;      auto y = bounds.top;      auto r = bounds.right;      auto b = bounds.bottom;      auto const a = M_PI/180.0;      cairo_new_sub_path(&_context);      cairo_arc(&_context, r-radius, y+radius, radius, -90*a, 0*a);      cairo_arc(&_context, r-radius, b-radius, radius, 0*a, 90*a);      cairo_arc(&_context, x+radius, b-radius, radius, 90*a, 180*a);      cairo_arc(&_context, x+radius, y+radius, radius, 180*a, 270*a);      cairo_close_path(&_context);   }   inline void canvas::circle(struct circle c)   {      arc(point{ c.cx, c.cy }, c.radius, 0.0, 2 * M_PI);   }   inline void canvas::fill_style(color c)   {      _state.fill_style = [this, c]()      {         cairo_set_source_rgba(&_context, c.red, c.green, c.blue, c.alpha);      };   }   inline void canvas::stroke_style(color c)   {      _state.stroke_style = [this, c]()      {         cairo_set_source_rgba(&_context, c.red, c.green, c.blue, c.alpha);      };   }   inline void canvas::line_width(float w)   {      cairo_set_line_width(&_context, w);   }   inline void canvas::shadow_style(point offs, float blur, color c)   {      assert(false); // unimplemented   }   inline void canvas::fill_style(linear_gradient const& gr)   {      _state.fill_style = [this, gr]()      {         cairo_pattern_t* pat =            cairo_pattern_create_linear(               gr.start.x, gr.start.y, gr.end.x, gr.end.y            );         for (auto cs : gr.space)         {            cairo_pattern_add_color_stop_rgba(               pat, cs.offset,               cs.color.red, cs.color.green, cs.color.blue, cs.color.alpha            );         }         cairo_set_source(&_context, pat);      };   }   inline void canvas::fill_style(radial_gradient const& gr)   {      _state.fill_style = [this, gr]()      {         cairo_pattern_t* pat =            cairo_pattern_create_radial(               gr.start.x, gr.start.y, gr.start_radius,               gr.end.x, gr.end.y, gr.end_radius            );         for (auto cs : gr.space)         {            cairo_pattern_add_color_stop_rgba(               pat, cs.offset,               cs.color.red, cs.color.green, cs.color.blue, cs.color.alpha            );         }         cairo_set_source(&_context, pat);      };   }   inline void canvas::linear_gradient::add_color_stop(color_stop cs)   {      space.push_back(cs);   }   inline void canvas::radial_gradient::add_color_stop(color_stop cs)   {      space.push_back(cs);   }   inline void canvas::fill_rect(struct rect r)   {      rect(r);      fill();   }   inline void canvas::fill_round_rect(struct rect r, float radius)   {      round_rect(r, radius);      fill();   }   inline void canvas::stroke_rect(struct rect r)   {      rect(r);      stroke();   }   inline void canvas::stroke_round_rect(struct rect r, float radius)   {      round_rect(r, radius);      stroke();   }   inline void canvas::font(char const* face, float size, int style)   {      cairo_font_slant_t slant = (style & italic) ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL;      cairo_font_weight_t weight = (style & bold) ? CAIRO_FONT_WEIGHT_BOLD :CAIRO_FONT_WEIGHT_NORMAL;      cairo_select_font_face(&_context, face, slant, weight);      cairo_set_font_size(&_context, size);   }   namespace   {      inline point get_text_start(cairo_t& _context, point p, int align, char const* utf8)      {         cairo_text_extents_t extents;         cairo_text_extents(&_context, utf8, &extents);         cairo_font_extents_t font_extents;         cairo_scaled_font_extents(cairo_get_scaled_font(&_context), &font_extents);         switch (align & 0x3)         {            case canvas::text_alignment::right:               p.x -= extents.width;               break;            case canvas::text_alignment::center:               p.x -= extents.width/2;               break;            default:               break;         }         switch (align & 0x1C)         {            case canvas::text_alignment::top:               p.y += font_extents.ascent;               break;            case canvas::text_alignment::middle:               p.y += font_extents.ascent/2 - font_extents.descent/2;               break;            case canvas::text_alignment::bottom:               p.y -= font_extents.descent;               break;            default:               break;         }         return p;      }   }   inline void canvas::fill_text(point p, char const* utf8)   {      p = get_text_start(_context, p, _state.align, utf8);      _state.fill_style();      cairo_move_to(&_context, p.x, p.y);      cairo_show_text(&_context, utf8);   }   inline void canvas::stroke_text(point p, char const* utf8)   {      p = get_text_start(_context, p, _state.align, utf8);      _state.stroke_style();      cairo_move_to(&_context, p.x, p.y);      cairo_text_path(&_context, utf8);      stroke();   }   inline canvas::text_metrics canvas::measure_text(char const* utf8)   {      cairo_text_extents_t extents;      cairo_text_extents(&_context, utf8, &extents);      cairo_font_extents_t font_extents;      cairo_scaled_font_extents(cairo_get_scaled_font(&_context), &font_extents);      return {         /*ascent=*/    float(font_extents.ascent),         /*descent=*/   float(font_extents.descent),         /*leading=*/   float(font_extents.height-(font_extents.ascent+font_extents.descent)),         /*size=*/      { float(extents.width), float(extents.height) }      };   }   inline void canvas::text_align(int align)   {      _state.align = align;   }   inline void canvas::draw(pixmap const& pm, photon::rect src, photon::rect dest)   {      auto  state = new_state();      auto  w = dest.width();      auto  h = dest.height();      auto  scale_ = point{ w/src.width(), h/src.height() };      cairo_identity_matrix(&_context);      translate(device_to_user(dest.top_left()));      scale(scale_);      cairo_set_source_surface(&_context, pm._surface, -src.left, -src.top);      rect({ 0, 0, w/scale_.x, h/scale_.y });      cairo_fill(&_context);   }   inline void canvas::draw(pixmap const& pm, photon::rect dest)   {      draw(pm, { 0, 0, pm.size() }, dest);   }   inline void canvas::draw(pixmap const& pm, point pos)   {      draw(pm, { 0, 0, pm.size() }, { pos, pm.size() });   }   inline canvas::state::state(canvas& cnv_)     : cnv(&cnv_)   {      cnv->save();   }   inline canvas::state::state(state&& rhs)    : cnv(rhs.cnv)   {      rhs.cnv = 0;   }   inline canvas::state::~state()   {      if (cnv)         cnv->restore();   }   inline canvas::state& canvas::state::operator=(state&& rhs)   {      cnv = rhs.cnv;      rhs.cnv = 0;      return *this;   }   inline void canvas::save()   {      cairo_save(&_context);      _state_stack.push(_state);   }   inline void canvas::restore()   {      _state = _state_stack.top();      _state_stack.pop();      cairo_restore(&_context);   }}#endif