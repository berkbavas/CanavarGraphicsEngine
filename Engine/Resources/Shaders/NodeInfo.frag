#version 330 core

uniform int nodeID;
uniform int meshID;
uniform bool fillVertexInfo = false;

flat in int fsVertexID; // Do not interpolate

layout (location = 0) out ivec4 nodeInfo;
layout (location = 1) out ivec4 vertexInfo;

void main()
{
     nodeInfo = ivec4(nodeID, meshID, 0, 1);

     if(fillVertexInfo)
        vertexInfo = ivec4(nodeID, meshID, fsVertexID, 1);
    else
        vertexInfo = ivec4(0, 0, 0, 0);
}
