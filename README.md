# LambPlanet, a Josh's Farm game

## Getting setup:
0.) Install git: https://git-scm.com/book/en/v2/Getting-Started-Installing-Git

1.) Clone the repository
```
cd [folder you want to put your enlistment in]
git clone https://github.com/optic5471/lambplanet
git config branch.autosetuprebase always
git config push.default simple
cd lambplanet
mkdir build
```
    
2.) Installing CMake 3.6:
  * On Windows:
```
1.) https://cmake.org/download/
2.) Select the appropriate installer for your device (version doesn't matter as long as it's at least 3.6)
```
  * On Linux: [DEPRECATED UNTIL LINUX SUPPORT]
```
 * apt-get version:
 1.) Get cmake and verify version
    sudo apt-get install cmake
    cmake --version
    (should be at least 3.6.0)

* apt-get doesn't have 3.6.0 or above?
1.) Uninstall current version if it's already installed
  sudo apt-get purge cmake
2.) Go to https://cmake.org/download/ and download and extract the latest version
  mkdir ~/temp
  cd ~/temp
  wget https://cmake.org/files/v3.6/cmake-3.6.0.tar.gz
  tar xzvf cmake-3.6.0.tar.gz
  cd cmake-3.6.0/
3.) Install it
  ./bootstrap
  make -j4
  sudo make install
4.) Test your new cmake version
  cmake --version
  Result should be something like: cmake version 3.6.0
```

3.) Installing Qt 5.9.0
```
1.) https://info.qt.io/download-qt-for-application-development
2.) When prompted for install directory, choose wherever you like. I will reference this location as `install_dir`
3.) When prompted for version, choose deselect all. Then, under Qt 5.9.0 select msvc2017 64 bit
4.) When it is finished installing add `install_dir\5.9\msvc2017_64\bin` to your path
5.) Add an environment variable called Qt defined as `install_dir\5.9`
```

4.) Installing Wwise 2017.1.0.6302
```
1.) Go to the Wwise website and download their launcher: https://www.audiokinetic.com/download/
2.) Open the launcher, you may need to make an account.
3.) There should be a big blue button that says "Instal Wwise 2017.1.0.6302" click it.
      - If not, navigate to install another version and find 2017.1.0.6302
4.) Besides the defaults, you will want:
    Packages:
      - SDK (C++)
    Deployment Platforms:
      - Under Microsoft, choose Windows
```

5.) Installing Vulkan 1.0.46.9
```
1.) Follow this link (https://vulkan.lunarg.com/sdk/home) and download the 1.0.46.0 installer for your platform
2.) Install, all of the default options should be fine
```

6.) Your first build
  * Follow the relevant instructions in the Building section below

## Building
* On Windows:
```
cd build
cmake ..
LambPlanet.sln

// Visual Studio should be open now
Right click LambPlanet and set as startup project
Build as per the usual
```

* On Linux: [DEPRECATED UNTIL LINUX SUPPORT]
```
cd build
cmake ..
make
cd LambPlanet
./LambPlanet
```

## Workflow
### Set up
#### 1. Get latest changes
```
git fetch
```
#### 2. Create your new branch
  * Some quick notes:
    * branch names must be all lower case [cross-platform development and Windows isn't case sensitive]
    * parent_branch is the branch you intend to integrate your changes back onto
    * feature_branch is the branch you are currently working on

```
git checkout parent_branch
git checkout -b feature_branch
git push -u origin feature_branch
```

#### 3. Do some development
  * Make changes, commit, and push them as many times as needed
  
#### 4. Code Review and Integration

1. Get the latest parent_branch
```
git checkout parent_branch
git fetch
git reset --hard origin/parent_branch
```
2. Rebase your commits on the parent_branch's history
```
Note: `git rebase -i --fork-point parent_branch` may result in merge conflicts. Run git mergetool to solve these conflicts
git checkout feature_branch
git rebase -i --fork-point parent_branch
git push --force-with-lease
```
3.  Create a new pull request
  * Go to https://github.com/optic5471/lambplanet
  * Click on the pull requests tab and click the new pull request button
  * Fill in the first drop down with your parent branch and the second with your feature branch [in this example that's parent_branch and feature_branch]
  
4. Respond to feedback. Make and commit changes as needed then submit for review again. Once you've gotten the greenlight, move on to the next step
  
5. Once your Code Review has been greenlit, on the page for your pull request there should be a squash and merge button
  * Click this button and fill out an appropriate commit
  * The first box should be describe what feature has been added
  * The second box should state who CR'd your code
  
```
// Example:

Made a feature for thing

[CR: None]

```

* if you're interested in manually squashing and merging ping me and I will actually document it here
  

## Optional Software / Settings
* Install posh-git [powershell only]
```
https://github.com/dahlbyk/posh-git
```
* Install git extensions
```
https://sourceforge.net/projects/gitextensions/
```
* Have git remember your credentials perminantly
```
git config --global credential.helper store
```
* Have git forget your credentials 
```
git config --global --unset credential.helper
```
