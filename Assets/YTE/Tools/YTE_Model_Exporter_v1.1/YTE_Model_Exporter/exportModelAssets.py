#-------------------------------------#
#    Model Assets Exporting v1.1      #
#                                     #
#    author: nicholas ammann          #
#-------------------------------------#

import os
import ntpath
import shutil

# import the maya commands
from maya import cmds
from maya import mel

# function gets called when export button pushed
def ExportFBX(directory, exportMode):
    
    scene = cmds.file(q=True, sceneName=True)

    file, ext = os.path.splitext(scene)

    # check if the extension is a maya binary
    if ext == ".mb":
        
        #build the fbx filename
        scene = scene.replace('.mb','.fbx')
        
        scene = ntpath.basename(scene)
        
        fullPath = directory + "\\" + scene
        
        # set the version of fbx exported
        mel.eval("FBXExportFileVersion -v FBX201400;")

        # don't generate a log file (not working?)
        mel.eval("FBXExportGenerateLog -v false;")

        if exportMode == "Scene":
            
            # Export the given fbx filename
            newFile = cmds.file(fullPath, exportAll=True, type="FBX export")

        elif exportMode == "Selected":
            
            # Export the given fbx filename
            newFile = cmds.file(fullPath, exportAll=False, exportSelected=True, type="FBX export")
#############################
            
def ExportTextures(directory, exportMode):
    
    if exportMode == "Scene":
    
        # Gets all 'file' nodes in maya
        fileList = cmds.ls(type="file")

        # For each file node..
        for f in fileList:

            # Get the name of the image attached to it
            fileName = cmds.getAttr(f + ".fileTextureName")

            if os.path.exists(fileName) == False:
                cmds.error(fileName + " does not exist.")
            
            # split the string into the filename and extension
            name, ext = os.path.splitext(fileName)
            
            if ext != ".png":
                cmds.error(fileName + " is not in PNG format.")

            # Copy it to the new directory
            shutil.copy(fileName, directory)

    elif exportMode == "Selected":

        meshes = cmds.ls(sl=True)
        
        for m in meshes:
            

            shader = cmds.listHistory(m, future=True)
            history = cmds.listHistory(shader)
            textureNodes = cmds.ls(history, type="file")

            for t in textureNodes:
                

                fileName = cmds.getAttr(t + ".fileTextureName")


                if os.path.exists(fileName) == False:
                    cmds.error(fileName + " does not exist.")

                name, ext = os.path.splitext(fileName)

                if ext != ".png":
                    cmds.error(fileName + " is not in PNG format.")

                # Copy it to the new directory
                shutil.copy(fileName, directory)

#############################

