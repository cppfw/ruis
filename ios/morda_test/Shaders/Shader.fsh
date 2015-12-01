//
//  Shader.fsh
//  morda_test
//
//  Created by Ivan on 28/11/15.
//  Copyright (c) 2015 org.igagis. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
