// Vertex shader
#version 420

//uniform mat4 MVP;
uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matModelInverseTranspose;	// For normal calculation


//uniform bool bUseVertexColour;		// Will default to GL_FALSE, which is zero (0)
//uniform vec3 vertexColourOverride;

in vec4 vColour;
in vec4 vPosition;
in vec4 vNormal;				// Vertex normal X,Y,Z (W ignored)
in vec4 vUVx2;					// 2 x Texture coords (vec4) UV0, UV1
in vec4 vTangent;				// For bump mapping X,Y,Z (W ignored)
in vec4 vBiNormal;				// For bump mapping X,Y,Z (W ignored)

// This was what we sent to the fragment shader:
//out vec4 fVertexColour;			// used to be "out vec3 color"
//out vec4 fVertWorldLocation;
//out vec4 fNormal;
//out vec4 fUVx2;
// Now we are sending data to the geometry shader:
out vec4 gVertexColour;			// used to be "out vec3 color"
out vec4 gVertWorldLocation;
out vec4 gNormal;
out vec4 gUVx2;

// For the height map example
uniform sampler2D heightMapTexture;
uniform bool bUseHeightMap;
uniform vec3 heightMapUVOffsetRotation;
uniform float heightMapScale;

uniform bool bBones;
uniform mat4 BoneMatrices[48];
uniform mat4 BoneRotationMatrices[48];
uniform vec4 in_BoneIDs;
uniform vec4 in_BoneWeights_x4;
out vec2 vBoneID;

void main()
{
	// Order of these is important
	//mvp = p * v * matModel; from C++ code
	
	mat4 MVP = matProjection * matView * matModel;
	
	vec4 vertPosition = vPosition;
	if (bBones)
	{
		vertPosition = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		vec4 normal = vec4(0.0f, 0.0f, 0.0f, 0.0f);

		if (vPosition.y < -0.5f)
		{
			vertPosition += BoneMatrices[int(in_BoneIDs[0])] * vPosition * in_BoneWeights_x4[0];
			vBoneID[0] = 0;
			normal += BoneRotationMatrices[int(in_BoneIDs[0])] * normalize(vPosition) * in_BoneWeights_x4[0];
		}
		else if (vPosition.y < 0.0f)
		{
			//vertPosition += BoneMatrices[int(in_BoneIDs[0])] * vPosition * in_BoneWeights_x4[0];
			vertPosition += BoneMatrices[int(in_BoneIDs[1])] * vPosition * in_BoneWeights_x4[1];
			vBoneID[0] = 1;
			normal += BoneRotationMatrices[int(in_BoneIDs[1])] * normalize(vPosition) * in_BoneWeights_x4[1];
		}
		else if (vPosition.y < 0.5f)
		{
			//vertPosition += BoneMatrices[int(in_BoneIDs[0])] * vPosition * in_BoneWeights_x4[0];
			//vertPosition += BoneMatrices[int(in_BoneIDs[1])] * vPosition * in_BoneWeights_x4[1];
			vertPosition += BoneMatrices[int(in_BoneIDs[2])] * vPosition * in_BoneWeights_x4[2];
			vBoneID[0] = 2;
			normal += BoneRotationMatrices[int(in_BoneIDs[2])] * normalize(vPosition) * in_BoneWeights_x4[2];
		}
		else if (vPosition.y <= 1.0f)
		{
			//vertPosition += BoneMatrices[int(in_BoneIDs[0])] * vPosition * in_BoneWeights_x4[0];
			//vertPosition += BoneMatrices[int(in_BoneIDs[1])] * vPosition * in_BoneWeights_x4[1];
			//vertPosition += BoneMatrices[int(in_BoneIDs[2])] * vPosition * in_BoneWeights_x4[2];
			vertPosition += BoneMatrices[int(in_BoneIDs[3])] * vPosition * in_BoneWeights_x4[3];
			vBoneID[0] = 3;
			normal += BoneRotationMatrices[int(in_BoneIDs[3])] * normalize(vPosition) * in_BoneWeights_x4[3];
		}
		else
		{
			vertPosition += BoneMatrices[int(in_BoneIDs[0])] * vPosition * in_BoneWeights_x4[0];
			vertPosition += BoneMatrices[int(in_BoneIDs[1])] * vPosition * in_BoneWeights_x4[1];
			vertPosition += BoneMatrices[int(in_BoneIDs[2])] * vPosition * in_BoneWeights_x4[2];
			vertPosition += BoneMatrices[int(in_BoneIDs[3])] * vPosition * in_BoneWeights_x4[3];
			vBoneID[0] = 4;
			normal += BoneRotationMatrices[int(in_BoneIDs[0])] * normalize(vPosition) * in_BoneWeights_x4[0];
			normal += BoneRotationMatrices[int(in_BoneIDs[1])] * normalize(vPosition) * in_BoneWeights_x4[1];
			normal += BoneRotationMatrices[int(in_BoneIDs[2])] * normalize(vPosition) * in_BoneWeights_x4[2];
			normal += BoneRotationMatrices[int(in_BoneIDs[3])] * normalize(vPosition) * in_BoneWeights_x4[3];
		}
		
//		vertPosition += BoneMatrices[int(in_BoneIDs[0])] * vPosition * in_BoneWeights_x4[0];
//		vertPosition += BoneMatrices[int(in_BoneIDs[1])] * vPosition * in_BoneWeights_x4[1];
//		vertPosition += BoneMatrices[int(in_BoneIDs[2])] * vPosition * in_BoneWeights_x4[2];
//		vertPosition += BoneMatrices[int(in_BoneIDs[3])] * vPosition * in_BoneWeights_x4[3];



		gl_Position = MVP * vertPosition; 		// Used to be: vec4(vPosition, 1.0f);	// Used to be vPos
	
		// The location of the vertex in "world" space (not screen space)
		gVertWorldLocation = matModel * vertPosition;
	
		// Copy the vertex colour to the fragment shader
		// (if you want the colours from the original file used)
		gVertexColour = vColour;		// Used to be vCol
	
//		normal += BoneRotationMatrices[int(in_BoneIDs[0])] * normalize(vPosition) * in_BoneWeights_x4[0];
//		normal += BoneRotationMatrices[int(in_BoneIDs[1])] * normalize(vPosition) * in_BoneWeights_x4[1];
//		normal += BoneRotationMatrices[int(in_BoneIDs[2])] * normalize(vPosition) * in_BoneWeights_x4[2];
//		normal += BoneRotationMatrices[int(in_BoneIDs[3])] * normalize(vPosition) * in_BoneWeights_x4[3];
		// Calculate the normal based on any rotation we've applied.
		// This inverse transpose removes scaling and tranlation (movement) 
		// 	from the matrix.
		//gNormal = matModelInverseTranspose * normalize(vNormal);
		gNormal = normal;
		gNormal = normalize(gNormal);
	
		// Copy the rest of the vertex values:
		gUVx2 = vUVx2;

		return;
	}
	
	if (bUseHeightMap)
	{

//       _                                                               _      _      _      _  _  _  
//      / \    __ _   __ _   __ _  _ __  _ __  _ __  __ _   __ _   __ _ | |__  | |__  | |__  | || || | 
//     / _ \  / _` | / _` | / _` || '__|| '__|| '__|/ _` | / _` | / _` || '_ \ | '_ \ | '_ \ | || || | 
//    / ___ \| (_| || (_| || (_| || |   | |   | |  | (_| || (_| || (_| || | | || | | || | | ||_||_||_| 
//   /_/   \_\\__,_| \__,_| \__,_||_|   |_|   |_|   \__, | \__, | \__, ||_| |_||_| |_||_| |_|(_)(_)(_) 
//                                                  |___/  |___/  |___/                                
//	So after several hours of thinking "but I've done this maybe 100 times!!! WHY ISN'T THIS WORKING!!!??!?!?!"		
//  I realized something very subtle...
//
//	Below is the original line I had. 
//  Since there was clearly some random (noisy) values being used - the "jumping around" on the surface,
//    I was assuming the binding of the textures wasn't happening (or something like that).
// 
//		vec2 sampleUV = fUVx2.xy + heightMapUVOffset.xy;
//
//  But I was wrong. "fUV..." is the variable LEAVING the vertex shader (as an "out").
//  But at this point in the shader, it isn't SET to anything. So it's random data. 
// 
//  The line should be using "vUV..." (with a "v"), which is ENTERING ("in") the shader. 
// 

		// "Shift" the texture around ("moving" the height map)
		vec2 sampleUV = vUVx2.xy + heightMapUVOffsetRotation.xy;
		
			// Apply a rotation around the centre location:
			float rotationOffset = heightMapUVOffsetRotation.z;
			// We do this in 3 steps:
			// 1. Translate to 0.5,0.5
			// 2. Rotate
			// 3. Translate BACK to 0,0
			//
			// BUUUUUUT, remember that matrix calculations go "backwards", so we 
			//	do these operations in the REVERSE order. 
			// (Another way to think about it, the LAST one you have in code 
			//  is the FIRST operation that happens mathematically.
			//

			// 3: Translate (BACK to the origin):
			sampleUV.xy -= vec2(0.5f, 0.5f);
			
			// 2: Rotate: in 2D. 
			// 
			// See: https://en.wikipedia.org/wiki/Rotation_matrix 
			//      
			//	R = | cos(rotationOffset)   -sin(rotationOffset) |
			//      | sin(rotationOffset)   cos(rotationOffset)  |
			// 
			//      https://www.khronos.org/opengl/wiki/Data_Type_(GLSL)#Matrix_constructors
			mat2 matRotate2D = 
				mat2( vec2( cos(rotationOffset), sin(rotationOffset) ), 	// 1st column
					  vec2( -sin(rotationOffset), cos(rotationOffset) ) );	// 2nd column
			
			sampleUV.xy = matRotate2D * sampleUV.xy;
			// 
			// 1. Translate to 0.5,0.5:
			sampleUV.xy += vec2(0.5f, 0.5f);
		
		// Since it's only sampling greyscale, I'm just sampling red (will return 0 to 1)
		float heightSample = texture(heightMapTexture, sampleUV.xy ).r;
		
		vertPosition.y += (heightSample * heightMapScale);
		
	}//if (bUseHeightMap)
	
			
    gl_Position = MVP * vertPosition; 		// Used to be: vec4(vPosition, 1.0f);	// Used to be vPos
	
	// The location of the vertex in "world" space (not screen space)
	gVertWorldLocation = matModel * vertPosition;
	
	// Copy the vertex colour to the fragment shader
	// (if you want the colours from the original file used)
    gVertexColour = vColour;		// Used to be vCol
	
	// Calculate the normal based on any rotation we've applied.
	// This inverse transpose removes scaling and tranlation (movement) 
	// 	from the matrix.
	gNormal = matModelInverseTranspose * normalize(vNormal);
	gNormal = normalize(gNormal);
	
	// Copy the rest of the vertex values:
	gUVx2 = vUVx2;

};