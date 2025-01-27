#!/bin/bash
# This is a script shell for deploying a meshlab-portable folder.
# Requires a properly built meshlab (see 1_build.sh).
#
# Without given arguments, the folder that will be deployed is meshlab/install.
#
# You can give as argument the path where meshlab is installed.

SCRIPTS_PATH="$(dirname "$(realpath "$0")")"
RESOURCES_PATH=$SCRIPTS_PATH/../../resources
INSTALL_PATH=$SCRIPTS_PATH/../../install
QT_DIR=""

#checking for parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
        INSTALL_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -qt=*|--qt_dir=*)
        QT_DIR=${i#*=}/bin/
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

${QT_DIR}windeployqt $INSTALL_PATH/meshlab.exe

${QT_DIR}windeployqt $INSTALL_PATH/plugins/filter_sketchfab.dll --libdir $INSTALL_PATH/

mv $INSTALL_PATH/lib/meshlab/IFX* $INSTALL_PATH
cp $INSTALL_PATH/IFXCoreStatic.lib $INSTALL_PATH/lib/meshlab/
cp $RESOURCES_PATH/LICENSE.txt $INSTALL_PATH/
cp $RESOURCES_PATH/privacy.txt $INSTALL_PATH/

if [ ! -f $INSTALL_PATH/vc_redist.x64.exe ]
then
    echo "Downloading vc_redist because it was missing..."
    wget https://aka.ms/vs/17/release/vc_redist.x64.exe --output-document=$INSTALL_PATH/vc_redist.x64.exe
fi
