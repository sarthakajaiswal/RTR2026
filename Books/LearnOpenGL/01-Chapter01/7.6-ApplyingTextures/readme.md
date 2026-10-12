Output the vertex position to the fragment shader using the out keyword and set the
fragment’s color equal to this vertex position (see how even the vertex position values are
interpolated across the triangle). Once you managed to do this; try to answer the following
question: why is the bottom-left side of our triangle black?

-> Because the left corner of triangle (-1.0f, -1.0f, 0.0f) has negative x and y coordinates which 
    are clamped to 0.0f while giving color. 
    same with all the fragments in 3rd quadrant i.e. from center of triangle to its left part. 