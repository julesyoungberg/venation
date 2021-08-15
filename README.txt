Venation Pattern Generator
==========================

This program simulates venation growth using a space colonization algorithm.

Let $TOP_DIR denote the directory containing this README file.
Let $INSTALL_DIR denote the directory into which this
software is to be installed.
To build and install the software, use the commands:
    cd $TOP_DIR
    cmake -H. -Btmp_cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR
    cmake --build tmp_cmake --clean-first --target install

To run a demonstration, use the commands:
    $INSTALL_DIR/bin/demo

Command Line Options:
  -h [ --help ]         produce help message
  --width arg           Simulation width in pixels. Defaults to 512.
  --height arg          Simulation height in pixels. Defaults to 512.
  --num-attractors arg  Number of random attractors to generate. Defaults to 
                        5000.
  --seeds arg           A list of 2D points to start growing from. Input should
                        be of the form "(x1,y2),...,(xn,yn)" where each x and y
                        is in the interval [-1, 1]. Defaults to (0,0).
  --mode arg            Growth mode, 'open' or 'closed' venation styles. 
                        Defaults to 'open'.
  --timeout arg         A time limit in seconds after which the simulation 
                        result will be saved to the output file and the program
                        will terminate. Defaults to 60 seconds.
  --growth-radius arg   The maximum distance an attractor can be from a growth 
                        node and still influence it (relative to normalized 
                        points). Defaults to 0.1.
  --growth-rate arg     The size of the step taken at each growth step 
                        (relative to normalized points). Defaults to 0.002.
  --consume-radius arg  The distance between an attractor and node at which 
                        point the attractor is considered consumed and removed 
                        (relative to normalized points). Defaults to 0.0005.
  --mask-shades arg     The number of grayscale shades to quantize the mask 
                        down to. Defaults to 2.
  --mask arg            A path to a pnm image file that will be used to mask 
                        the attractors. i.e. generated attractors will only be 
                        kept for the simulation if the corresponding pixel in 
                        the image is bright enough. The file is converted to 
                        grayscale and quantized into `mask-shades` shades. The 
                        grayscale value is then used as a probability that an 
                        attractor at that position will be kept.
  --outfile arg         An image path to store the result at. The path must 
                        include an extension and it must be pnm.
