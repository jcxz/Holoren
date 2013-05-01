%%
% A function to load binary png image (i.e. containing only black and white pixels)
% and convert it to bitmap image in format of 1 bit per pixel
function convert_png_to_1bit_bmp(png_path, bmp_path)
  png = imread(png_path, 'png');
  imwrite(logical(png), bmp_path, 'bmp');
end