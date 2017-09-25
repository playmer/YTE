import os
import ntpath
import shutil

# import all the Maya
from maya import cmds
from maya import mel
from maya import OpenMayaUI as mui

#import nice Qt smiles
from PySide2.QtCore import *
from PySide2.QtGui import *
from PySide2.QtWidgets import *
from PySide2 import __version__
from shiboken2 import wrapInstance

def AskForSaveFolder():
    # get the current path
    projectPath = cmds.workspace(fullName=True)
    
    # have the user choose a folder to save to
    direc = QFileDialog.getExistingDirectory(None, 'Select Folder', projectPath)
    
    return direc
##########################


def CheckMaterialNames():

### start from stack overflow
# https://stackoverflow.com/questions/41748884/getting-all-materials-attached-to-object-throughout-all-render-layers-maya
    
    # get all meshes
    meshList = cmds.ls(type="mesh")
    
    # initialize array to store shaders in
    shaderList = {}
    
    # initialize an array for the shaders (stored under the mesh name)
    for mesh in meshList:
        shaderList[mesh]={}
        
    
    # store all the render layers
    renderLayers = [x for x in cmds.ls(type="renderLayer")]
    
    for layer in renderLayers:
        
        layerName = layer
        
        attrib = layerName+'.outAdjustments'
        
        attr = cmds.getAttr(attrib, multiIndices=True)
        
        if attr != None:
        
            numConnectedMeshes = len()
        
            for meshIndex in range(numConnectedMeshes):
            
                queryConnection = layerName+".outAdjustments["+str(meshIndex)+"]"
            
                if cmds.listConnections(queryConnection+".outPlug") != None:
                
                    currTransform = cmds.listConnections(queryConnection+".outPlug")[0]
                
                    currentMeshName = cmds.listRelatives(currTransform)[0]
                
                    currMeshShadingGroup = cmds.listConnections(queryConnection+".outValue")[0]
                
                    currShadingGroupSurfaceShader = cmds.listConnections(currMeshShadingGroup+".surfaceShader")[0]
                
                    shaderList[currMeshName][layerName] = currShadingGroupSurfaceShader
                
    for mesh in shaderList:
        
        if shaderList[mesh]=={}:
            layerName = "defaultRenderLayer"
            
            materialAppliedToObject = cmds.ls(
                                      cmds.listConnections(
                                      cmds.listHistory(cmds.listRelatives(mesh, parent=1))), mat=1)
                
            shaderList[mesh][layerName] = materialAppliedToObject
### end from stack overflow
    
    
    # parse the array of shader names
    # each mesh name contains an array of render layers paired with shader names
    for mesh in shaderList:
      
        for layerKey, materials in shaderList[mesh].items():
            
            for mat in materials:
            
                if mat[:4] != "All_" and mat[:5] != "None_" and mat[:7] != "NoNorm_" and mat[:7] != "NoSpec_" and mat[:9] != "OnlyDiff_":
                    cmds.error("Material " + mat + " on mesh " + mesh + " does not meet YTE naming conventions.")
    
    return
##########################

# function gets called when export button pushed
def ExportFBX(directory):
    
    # get the current path
    projectFiles = cmds.workspace(l=True)

    # loop through the file and folder names
    for t in projectFiles:
    
        # split the string into the filename and extension
        name, ext = os.path.splitext(t)
    
        # check if the extension is a maya binary
        if ext == ".mb":
            
            scene = cmds.file(q=True, sceneName=True)
            
            #build the fbx filename
            scene = scene.replace('.mb','.fbx')
            
            scene = ntpath.basename(scene)
            
            fullPath = directory + "\\" + scene
            
            # set the version of fbx exported
            mel.eval("FBXExportFileVersion -v FBX201400")

            # Export the given fbx filename
            newFile = maya.cmds.file(fullPath, exportAll=True, type="FBX export")
#############################
            
def ExportTextures(directory):
    # Gets all 'file' nodes in maya
    fileList = cmds.ls(type='file')

    # For each file node..
    for f in fileList:

        # Get the name of the image attached to it
        fileName = cmds.getAttr(f + '.fileTextureName')
        
        # split the string into the filename and extension
        name, ext = os.path.splitext(fileName)
        
        if ext != ".png":
            cmds.error("Texture not in PNG format. : " + fileName)

        # Copy it to the new directory
        shutil.copy(fileName, directory)
#############################

CheckMaterialNames()
saveFolder = AskForSaveFolder()
ExportFBX(saveFolder)
ExportTextures(saveFolder)