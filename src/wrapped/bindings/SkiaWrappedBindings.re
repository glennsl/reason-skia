open Ctypes;

module SkiaTypes = SkiaWrappedBindingsTypes.M(Skia_wrapped_generated_type_stubs);

// TODO Would it make sense to make all nullable parameters ptr_opt?
// This might become beneficial if we decide to make all the structs completely
// abstract instead of being a pointer to an abstract type
// It would also be consistent with the fact that we already hide memory mgmt
// TODO add a module interface for each module
module M = (F: FOREIGN) => {
  open F;

  module Color = {
    type t;

    let makeArgb = foreign(
      "reason_skia_stub_sk_color_set_argb",
      int @-> int @-> int @-> int @-> returning(uint32_t),
    );
  };

  module Paint = {
    type t = ptr(SkiaTypes.Paint.t);
    type style = SkiaTypes.Paint.style;

    let make = () => {
      let allocatePaint = foreign("sk_paint_new", void @-> returning(t));
      let freePaint = foreign("sk_paint_delete", t @-> returning(void));
      let paint = allocatePaint();
      Gc.finalise(paint, freePaint);
      paint;
    };

    let setColor =
      foreign("sk_paint_set_color", t @-> Color.t @-> returning(void));
    let setAntiAlias = 
      foreign("sk_paint_set_antialias", t @-> bool @-> returning(void));
    let setStyle = 
      foreign("sk_paint_set_style", t @-> style @-> returning(void));
    let setStrokeWidth = 
      foreign("sk_paint_set_stroke_width", t @-> float @-> returning(void));
  };

  module Rect = {
    type t = ptr(SkiaTypes.Rect.t);

    let makeEmpty = () => {
      let rect = allocate_n(SkiaTypes.Rect.t, 1);
      setf(rect, SkiaTypes.Rect.left, 0.);
      setf(rect, SkiaTypes.Rect.top, 0.);
      setf(rect, SkiaTypes.Rect.right, 0.);
      setf(rect, SkiaTypes.Rect.bottom, 0.);
      rect;
    };
    let makeLtrb = (left, top, right, bottom) => {
      let rect = allocate_n(SkiaTypes.Rect.t, 1);
      setf(rect, SkiaTypes.Rect.left, left);
      setf(rect, SkiaTypes.Rect.top, top);
      setf(rect, SkiaTypes.Rect.right, right);
      setf(rect, SkiaTypes.Rect.bottom, bottom);
      rect;
    };
  };

  module Path = {
    type t = ptr(SkiaTypes.Path.t);

    let make = () => {
      let allocatePath = foreign("sk_path_new", void @-> returning(t));
      let freePath = foreign("sk_path_delete", t @-> returning(void));
      let path = allocatePath();
      Gc.finalise(path, freePath);
      path;
    };

    let moveTo =
      foreign("sk_path_move_to", t @-> float @-> float @-> returning(void));
    let lineTo =
      foreign("sk_path_line_to", t @-> float @-> float @-> returning(void));
    let cubicTo =
      foreign(
        "sk_path_cubic_to",
        t @->
        float @->
        float @->
        float @->
        float @->
        float @->
        float @->
        returning(void),
      );
  };

  module Data = {
    type t = ptr(SkiaTypes.Data.t);

    let makeString = (data) => {
      let getData = foreign("sk_data_get_data", t @-> returning(ptr(void)));
      let getSize = foreign("sk_data_get_size", t @-> returning(size_t));
      string_from_ptr(from_voidp(getData(data)), getSize(data));
    };
  };

  module Imageinfo = {
    type t = ptr(SkiaTypes.Imageinfo.t);
    
    let make = (width, height, colorType, alphaType, colorspace) => {
      let imageinfo = allocate_n(SkiaTypes.Imageinfo.t, 1);
      setf(imageinfo, SkiaTypes.Imageinfo.width, width);
      setf(imageinfo, SkiaTypes.Imageinfo.height, height);
      setf(imageinfo, SkiaTypes.Imageinfo.colorType, colorType);
      setf(imageinfo, SkiaTypes.Imageinfo.alphaType, alphaType);
      setf(imageinfo, SkiaTypes.Imageinfo.colorspace, colorspace);
      imageinfo;
    };
  };

  module Image = {
    type t = ptr(SkiaTypes.Image.t);

    let encodeToData = (image) => {
      let encodeImage = foreign("sk_image_encode", t @-> returning(Data.t));
      let freeData = foreign("sk_data_unref", Data.t @-> returning(void));
      let data = encodeImage(image);
      Gc.finalise(data, freeData);
      data;
    };
  };

  module Canvas = {
    type t = ptr(SkiaTypes.Canvas.t);

    let drawPaint =
      foreign("sk_canvas_draw_paint", t @-> Paint.t @-> returning(void));
    let drawRect =
      foreign(
        "sk_canvas_draw_rect",
        t @-> Rect.t @-> Paint.t @-> returning(void),
      );
    let drawOval =
      foreign(
        "sk_canvas_draw_oval",
        t @-> Rect.t @-> Paint.t @-> returning(void),
      );
    let drawPath =
      foreign(
        "sk_canvas_draw_path",
        t @-> Path.t @-> Paint.t @-> returning(void),
      );
  };

  module SurfaceProps = {
    type t = ptr(SkiaTypes.SurfaceProps.t);
  };

  module Surface = {
    type t = ptr(SkiaTypes.Surface.t);
      
    let makeRaster = (imageinfo, rowBytes, surfaceProps) => {
      let allocateRasterSurface =
        foreign(
          "sk_surface_new_raster",
          Imageinfo.t @-> size_t @-> ptr_opt(SkiaTypes.SurfaceProps.t) @-> returning(t),
        );
      let freeSurface = foreign("sk_surface_unref", t @-> returning(void));
      let surface = allocateRasterSurface(imageInfo, rowBytes, surfaceProps);
      Gc.finalise(surface, freeSurface);
      surface;
    };

    let getCanvas =
      foreign("sk_surface_get_canvas", t @-> returning(Canvas.t));
    let makeImageSnapshot = (surface) => {
      let allocateImageSnapshot = foreign("sk_surface_new_image_snapshot", t @-> returning(Image.t));
      let freeImage = foreign("sk_image_unref", Image.t @-> returning(void));
      let image = allocateImageSnapshot(surface);
      Gc.finalise(image, freeImage);
      image;
    }
  };
};