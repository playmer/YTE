#-------------------------------------#
#    YTE Model Exporter v1.1          #
#                                     #
#    author: nicholas ammann          #
#-------------------------------------#

import os
import ntpath
import shutil

# import all the Maya
from maya import cmds
from maya import mel
from maya import OpenMayaUI as mui

# import nice Qt smiles
from PySide2.QtCore import *
from PySide2.QtGui import *
from PySide2.QtWidgets import *
from PySide2 import __version__
from shiboken2 import wrapInstance

# import my modules
try:
    from YTE_Model_Exporter import materialChecking as mchk
    from YTE_Model_Exporter import exportModelAssets as ema
except:
    import materialChecking as mchk
    import exportModelAssets as ema

# make sure the damn modules reload
reload(mchk)
reload(ema)


class ExportToolWindow(QWidget):

    def AskForSaveFolder(self):
        
        # get the current path
        projectPath = cmds.workspace(fullName=True)
        
        # have the user choose a folder to save to
        direc = QFileDialog.getExistingDirectory(None, 'Select Folder', projectPath)

        return direc
        

    def Export(self, exportMode):

        materialsTest = mchk.CheckMaterialNames(self, exportMode)

        if materialsTest == True:

            direc = self.AskForSaveFolder()

            if len(direc) == 0:

                error = "<span style=\" color:#ff0000;\" >"
                error += "No save folder was selected."
                error += "</span>"

                self.mTextEdit.append(error)
                
            else:

                if materialsTest == True:
                    ema.ExportFBX(direc, exportMode)
                    ema.ExportTextures(direc, exportMode)


    def ExportScene(self):
        self.Export("Scene")
            

    def ExportSelected(self):
        self.Export("Selected")


    def ClearText(self):
        self.mTextEdit.clear()


    def __init__(self):
        
        QWidget.__init__(self)

        self.resize(400, self.height())

        self.mLayout = QVBoxLayout(self)
        
        self.mExportSceneButton = QPushButton("Export Scene")
        self.mExportSceneButton.clicked.connect(self.ExportScene)
        self.mLayout.addWidget(self.mExportSceneButton)

        self.mExportSelectedButton = QPushButton("Export Selected")
        self.mExportSelectedButton.clicked.connect(self.ExportSelected)
        self.mLayout.addWidget(self.mExportSelectedButton)

        self.mClearTextButton = QPushButton("Clear Error Messages")
        self.mClearTextButton.clicked.connect(self.ClearText)
        self.mLayout.addWidget(self.mClearTextButton)

        self.mTextEdit = QTextEdit(self)
        self.mTextEdit.setReadOnly(True)
        self.mTextEdit.setLineWrapMode(QTextEdit.NoWrap)
        self.mLayout.addWidget(self.mTextEdit)
        
        self.setLayout(self.mLayout)
        self.show()

        return

    
##########################


if __name__ == "__main__":
    
    win = ExportToolWindow()
    
    

def run():
    
    win = ExportToolWindow()

    return win

	

	