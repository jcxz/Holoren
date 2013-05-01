%%
% A function to compare the bmp image stored by `convert_png_to_1bit_bmp`
% function with the original png image
function compare_png_with_1bit_bmp(png_path, bmp_path)
  % read images
  png = imread(png_path, 'png');
  bmp = imread(bmp_path, 'bmp');
  
  % convert them to a common format
  png_img = im2uint8(png);
  bmp_img = im2uint8(bmp);
  
  % and comapre them
  c = corr2(png_img, bmp_img);  % finding the correlation btwn two images
  if c == 1
    disp('The images are same.');
  else
    disp('The images are NOT same.');
  end;
end