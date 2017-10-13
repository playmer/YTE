#-------------------------------------#
#    Material Checking Module v1.1    #
#                                     #
#    author: nicholas ammann          #
#-------------------------------------#

from maya import cmds

try:
    from YTE_Model_Exporter import YTE_Model_Exporter
except:
    import YTE_Model_Exporter

def CheckForDiffuse(materialName):
    
    attr = materialName + ".color"

    diffuseMap = cmds.listConnections(attr, destination=True)
    
    hasDiffuse = False

    if diffuseMap != None:
        hasDiffuse = cmds.objectType(diffuseMap, isType="file")
    
    return hasDiffuse


def CheckForNormal(materialName):
    
    hasNormal = False
    
    attribs = cmds.ls(type="bump2d")
    
    for a in attribs:

        con = cmds.listConnections(a)

        for c in con:
            if c == materialName:
                hasNormal = True

    return hasNormal
    
    
def CheckForSpecular(materialName):
    
    hasSpecular = False
    
    attr = materialName + ".specularColor"
   
    try:
        spec = cmds.listConnections(attr, destination=True)
        
        hasSpecular = cmds.objectType(spec, isType="file")
        
    except:
        return hasSpecular;
    
    return hasSpecular



## helper functions for checking material names and maps ##
def FindMaps(materialName):

    hasDiffuse = CheckForDiffuse(materialName)

    hasNormal = CheckForNormal(materialName)

    hasSpecular = CheckForSpecular(materialName)

    maps = { "Diffuse" : hasDiffuse,
             "Normal" : hasNormal,
             "Specular" : hasSpecular }
    
    return maps


def CheckMaterial_All(material):
    
    maps = FindMaps(material)

    #container for storing missing maps for printing
    errors = []

    if maps["Diffuse"] == False:
        errors.append("Missing the diffuse map.")

    if maps["Normal"] == False:
        errors.append("Missing the normal map.")
    
    if maps["Specular"] == False:
        errors.append("Missing the specular map.")
        
    return errors
    
def CheckMaterial_None(material):
    
    maps = FindMaps(material)
     
    #container for storing missing maps for printing
    errors = []

    if maps["Diffuse"] == True:
        errors.append("Should not have a diffuse map.")

    if maps["Normal"] == True:
        errors.append("Should not have a normal map.")
    
    if maps["Specular"] == True:
        errors.append("Should not have a specular map.")
        
    return errors

def CheckMaterial_NoNorm(material):
    
    maps = FindMaps(material)

    #container for storing missing maps for printing
    errors = []

    if maps["Diffuse"] == False:
        errors.append("Missing the diffuse map.")

    if maps["Normal"] == True:
        errors.append("Should not have a normal map.")
    
    if maps["Specular"] == False:
        errors.append("Missing the specular map.")
        
    return errors

def CheckMaterial_NoSpec(material):
    
    maps = FindMaps(material)

    #container for storing missing maps for printing
    errors = []

    if maps["Diffuse"] == False:
        errors.append("Missing the diffuse map.")

    if maps["Normal"] == False:
        errors.append("Missing the normal map.")
    
    if maps["Specular"] == True:
        errors.append("Should not have a specular map.")
        
    return errors

def CheckMaterial_OnlyDiff(material):
    
    maps = FindMaps(material)
    
    #container for storing missing maps for printing
    errors = []

    if maps["Diffuse"] == False:
        errors.append("Missing the diffuse map.")

    if maps["Normal"] == True:
        errors.append("Should not have a normal map.")
    
    if maps["Specular"] == True:
        errors.append("Should not have a specular map.")
        
    return errors


materialTypes = { "All" : CheckMaterial_All,
                  "None" : CheckMaterial_None,
                  "NoNorm" : CheckMaterial_NoNorm,
                  "NoSpec" : CheckMaterial_NoSpec,
                  "OnlyDiff" : CheckMaterial_OnlyDiff }




def CheckMaterialNames(toolWindow, exportMode):
### start from stack overflow
# https://stackoverflow.com/questions/41748884/getting-all-materials-attached-to-object-throughout-all-render-layers-maya
    
    meshList = []

    if exportMode == "Scene":
        # get all meshes in the scene
        meshList = cmds.ls(type="mesh")

    elif exportMode == "Selected":
        # get only the currently selected meshes
        meshList = cmds.ls(sl=True)

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

            relatives = cmds.listRelatives(mesh, parent=True)
            
            if relatives != None:
                materialAppliedToObject = cmds.ls(
                                          cmds.listConnections(
                                          cmds.listHistory(relatives)), mat=True)
                    
                shaderList[mesh][layerName] = materialAppliedToObject
### end from stack overflow
    
    materialOK = True

    # parse the array of shader names
    # each mesh name contains an array of render layers paired with shader names
    for mesh in shaderList:
      
        for layerKey, materials in shaderList[mesh].items():
            
            for mat in materials:

                tokens = mat.split("_")

                try:

                    # try to call the appropriate function in the map
                    errors = materialTypes[tokens[0]](mat)

                    if errors:
                        
                        materialOK = False

                        for e in errors:
                            
                            message = "<span style=\" color:#ff0000;\" >"
                            message += mat + " : " + e
                            message += "</span>"

                            toolWindow.mTextEdit.append(message)
                       

                except:
                    # material doesn't match any naming convention
                    otherString = "<span style=\" color:#ff0000;\" >"
                    otherString += mat + " on mesh " + mesh + " does not match any YTE naming convention."
                    otherString += "</span>"
                    toolWindow.mTextEdit.append(otherString)
                    materialOK = False


    return materialOK

##########################


