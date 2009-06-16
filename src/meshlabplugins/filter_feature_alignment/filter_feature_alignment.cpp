/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include <Qt>
#include <QtGui>
#include <feature_alignment.h>
#include <filter_feature_alignment.h>
#include <feature_msc.h>
#include <feature_rgb.h>
#include <meshlabplugins/edit_pickpoints/pickedPoints.h>

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <cmath>
#include <stdio.h>

using namespace std;
using namespace vcg;

GLLogStream *glLog;
void mylogger(const char * f, ... )
{
    char buf[4096];
    va_list marker;
    va_start( marker, f );

    vsprintf(buf,f,marker);
    va_end( marker );
    glLog->Log(GLLogStream::DEBUG,buf);
}

FilterFeatureAlignment::FilterFeatureAlignment()
{
    typeList << AF_COMPUTE_FEATURE
             << AF_EXTRACTION
             << AF_MATCHING
             << AF_RIGID_TRANSFORMATION             
             << AF_CONSENSUS
             << AF_RANSAC
             << AF_RANSAC_DIAGRAM                                   
             << AF_PERLIN_COLOR
             << AF_COLOR_NOISE;

    FilterIDType tt;
    foreach(tt , types())
        actionList << new QAction(filterName(tt), this);
}

FilterFeatureAlignment::~FilterFeatureAlignment()
{
    for (int i = 0; i < actionList.count() ; i++ )
        delete actionList.at(i);
}

const MeshFilterInterface::FilterClass FilterFeatureAlignment::getClass(QAction *a)
{
    switch(ID(a))
    {
        case AF_COMPUTE_FEATURE :
        case AF_EXTRACTION :
        case AF_MATCHING :
        case AF_RIGID_TRANSFORMATION :        
        case AF_CONSENSUS:        
        case AF_PERLIN_COLOR:
        case AF_COLOR_NOISE:                
        case AF_RANSAC_DIAGRAM:        
        case AF_RANSAC : return MeshFilterInterface::RangeMap;
        default: assert(0);
    }
}

bool FilterFeatureAlignment::autoDialog(QAction *a)
{
  switch(ID(a))
  {
    default : return true;
  }
  assert(0);
}

const QString FilterFeatureAlignment::filterName(FilterIDType filter) const
{
    switch(filter)
    {
        case AF_COMPUTE_FEATURE : return "Compute feature";
        case AF_EXTRACTION : return "Features extraction";
        case AF_MATCHING : return "Features matching";
        case AF_RIGID_TRANSFORMATION : return "Rigid transformation";
        case AF_CONSENSUS : return "Consensus";
        case AF_RANSAC : return "Feature Alignment";
        case AF_RANSAC_DIAGRAM : return "Alignment Diagram";
        case AF_PERLIN_COLOR : return "Perlin color";
        case AF_COLOR_NOISE : return "Color Noise";
        default: assert(0);
    }
    return QString("error!");
}

const QString FilterFeatureAlignment::filterInfo(FilterIDType filterId) const
{
    switch(filterId)
    {
        case AF_COMPUTE_FEATURE : return "Compute feature values";
        case AF_EXTRACTION : return "Given a mesh and a valid feature type, extract the best k features from the mesh.";
        case AF_MATCHING : return "";
        case AF_RIGID_TRANSFORMATION : return "";
        case AF_CONSENSUS : return "Performs a randomized consensus, followed by a full consensus only if randomized consensus exceeds a given percentage.";
        case AF_RANSAC : return "Perform ransac algorithm";
        case AF_RANSAC_DIAGRAM : return "Ransac Diagram";        
        case AF_PERLIN_COLOR : return "Colors randomly the vertexes of the mesh";
        case AF_COLOR_NOISE : return "";
        default: assert(0);
    }
    return QString("error!");
}

const int FilterFeatureAlignment::getRequirements(QAction *action)
{
    return 0;
}

void FilterFeatureAlignment::initParameterSet(QAction *a, MeshDocument& md, FilterParameterSet & par)
{
    switch(ID(a))
    {
        case AF_COMPUTE_FEATURE:
        {
            QStringList l;
            l << "GMSmooth Curvature"
              << "FeatureRGB";
            par.addEnum("featureType", 0, l,"Feature type", "");
            break;
        }
        case AF_EXTRACTION:
        {
            QStringList l, st;
            l << "GMSmooth Curvature"
              << "FeatureRGB";
            st << "Uniform" << "Poisson";
            par.addEnum("featureType", 0, l,"Feature type", "");
            par.addInt("numFixFeatureSelected", 250, "Number of feature to select", "");
            par.addEnum("samplingStrategy", 0, st,"Sampling", "");
            par.addBool("pickPoints", false, "Store features as picked points", "");
            break;
        }
        case AF_MATCHING:
        {
            QStringList l;
            l << "GMSmooth Curvature"
              << "FeatureRGB";
            par.addEnum("featureType", 0, l,"Feature type: ", "");
            par.addMesh("mFix", 0, "Fix mesh");
            par.addMesh("mMov", 1, "Move mesh");
            par.addInt("numFixFeatureSelected", 250, "Number of feature extracted from fix mesh: ", "");
            par.addInt("nBase", 4, "Number of base features: ", "");            
            par.addInt("k", 150, "Number of neighboor picked: ", "");            
            break;
        }
        case AF_RIGID_TRANSFORMATION :
        {
            QStringList l;
            l << "GMSmooth Curvature"
              << "FeatureRGB";
            par.addEnum("featureType", 0, l,"Feature type: ", "");
            par.addMesh("mFix", 0, "Fix mesh");
            par.addMesh("mMov", 1, "Move mesh");
            par.addInt("numFixFeatureSelected", 250, "Number of feature extracted from fix mesh: ", "");
            par.addInt("nBase", 4, "Number of base features: ", "");           
            par.addInt("k", 150, "Number of neighboor picked: ", "");            
            break;
        }        
        case AF_CONSENSUS :
        {
            QStringList l;
            l << "GMSmooth Curvature"
              << "FeatureRGB";
            par.addEnum("featureType", 0, l,"Feature type: ", "");
            par.addMesh("mFix", 0, "Fix mesh");
            par.addMesh("mMov", 1, "Move mesh");
            par.addFloat("consensusDist", 2, "Consensus distance in \% of BBox diagonal of move mesh", "");
            par.addInt("fullConsensusSamples", 2500, "Number of samples", "");
            par.addBool("normEq", true, "Use normal equalization in consensus","");
            par.addBool("paint", true, "Paint move mesh according to consensus","");
            break;
        }
        case AF_RANSAC:
        {
            QStringList l;
            l << "GMSmooth Curvature"
              << "FeatureRGB";
            par.addEnum("featureType", 0, l,"Feature type", "");
            par.addMesh("mFix", 0, "Fix mesh");
            par.addMesh("mMov", 1, "Move mesh");
            par.addInt("ransacIter", 5000, "Max number of RANSAC iterations", "");
            par.addFloat("overlap", 75.0, "Overlap in \%", "");
            par.addFloat("consensusDist", 2, "Consensus distance in \% of BBox diagonal of move mesh", "");
            par.addInt("k", 150, "Number of neighboor picked: ", "");                      
            par.addBool("pickPoints", true, "Store base and match as picked points","");
            break;
        }
        case AF_RANSAC_DIAGRAM:
        {
            QStringList l;
            l << "GMSmooth Curvature"
              << "FeatureRGB";
            par.addEnum("featureType", 0, l,"Feature type", "");
            par.addMesh("mFix", 0, "Fix mesh");
            par.addMesh("mMov", 1, "Move mesh");
            par.addInt("ransacIter", 5000, "Max number of RANSAC iterations", "");
            par.addFloat("overlap", 75.0, "Overlap in \%", "");
            par.addFloat("consensusDist", 2, "Consensus distance in \% of BBox diagonal of move mesh", "");
            par.addInt("k", 150, "Number of neighboor picked: ", "");
            par.addInt("trials", 100, "Number of try", "");
            par.addInt("from", 1000, "From iteration", "");
            par.addInt("to", 5000, "To iteration", "");
            par.addInt("step", 1000, "Step", "");
            break;
        }                 
        case AF_PERLIN_COLOR:
        {
            par.addDynamicFloat("freq", 10.0f, 0.1f, 50.0f,"Frequency","");
            break;
        }
        case AF_COLOR_NOISE:
        {            
            par.addInt("noiseBits", 1, "Bits of noise","");
            break;
        }        
        default: assert(0);
    }
}

template<class MESH_TYPE, class ALIGNER_TYPE>
void FilterFeatureAlignment::setAlignmentParameters(MESH_TYPE& mFix, MESH_TYPE& mMov, FilterParameterSet& par, typename ALIGNER_TYPE::Parameters& param)
{
    typedef MESH_TYPE MeshType;
    typedef ALIGNER_TYPE AlignerType;    

    if(par.hasParameter("samplingStrategy")) param.samplingStrategy = par.getEnum("samplingStrategy");
    if(par.hasParameter("numFixFeatureSelected")) param.numFixFeatureSelected = par.getInt("numFixFeatureSelected");
    if(par.hasParameter("nBase")) param.nBase = par.getInt("nBase");
    if(par.hasParameter("k")) param.k = par.getInt("k");
    if(par.hasParameter("ransacIter")){ param.ransacIter = par.getInt("ransacIter"); if(param.ransacIter<0) param.ransacIter = 0;}
    if(par.hasParameter("fullConsensusSamples")) math::Clamp(param.fullConsensusSamples = par.getInt("fullConsensusSamples"),1,mMov.VertexNumber());
    if(par.hasParameter("overlap")) param.overlap = math::Clamp<float>(par.getFloat("overlap"),0.0f,100.0f);
    if(par.hasParameter("consensusDist")) param.consensusDist = math::Clamp<float>(par.getFloat("consensusDist"),0.0f,100.0f);
    if(par.hasParameter("pickPoints")) param.pickPoints = par.getBool("pickPoints");
    if(par.hasParameter("normEq")) param.normalEqualization = par.getBool("normEq");
    if(par.hasParameter("paint")) param.paint = par.getBool("paint");
}

bool FilterFeatureAlignment::applyFilter(QAction *filter, MeshDocument &md, FilterParameterSet & par, vcg::CallBackPos * cb)
{
    glLog=log;  //assign log to a global variable. needed to write the log from everywhere in the code

    //define needed typedef MeshType
    typedef CMeshO MeshType;

    //declare variables for parameters
    int featureType, from, to, step, trials;
    MeshModel *mFix, *mMov, *currMesh;
    //read parameters
    currMesh = md.mm(); //get current mesh from document
    if(par.hasParameter("mFix")) mFix = par.getMesh("mFix"); else mFix = currMesh;
    if(par.hasParameter("mMov")) mMov = par.getMesh("mMov"); else mMov = currMesh;
    if(par.hasParameter("featureType")) featureType = par.getEnum("featureType"); else featureType = -1;
    if(par.hasParameter("trials")) trials = par.getInt("trials"); else trials = 100;
    if(par.hasParameter("from")) from = par.getInt("from"); else from = 1000;
    if(par.hasParameter("to")) to = par.getInt("to"); else to = 5000;
    if(par.hasParameter("step")) step = par.getInt("step"); else step = 1000;

    switch(ID(filter))
    {
        case AF_COMPUTE_FEATURE:
        {            
            switch(featureType){
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    FeatureType::Parameters param;
                    FeatureType::SetupParameters(param);
                    return ComputeFeatureOperation<MeshType,FeatureType>(*currMesh, param, cb);
                }
                case 1:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    FeatureType::Parameters param;
                    FeatureType::SetupParameters(param);
                    return ComputeFeatureOperation<MeshType,FeatureType>(*currMesh, param, cb);
                }                
                default: assert(0);
            }   //end switch(ftype)
            return true;
        }   //end case AF_COMPUTE_FEATURE
        case AF_EXTRACTION:
        {                      
            //set error message just once to avoid code grow long...
            errorMessage = QString("Probably features have not been computed for this mesh.\n");

            switch(featureType){
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 3> FeatureType; //define needed typedef FeatureType                    
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<MeshType, AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    bool ok = ExtractionOperation<MeshType, FeatureType, AlignerType>(*currMesh, alignerParam, cb);
                    return ok;
                }
                case 1:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType                    
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<MeshType, AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    bool ok = ExtractionOperation<MeshType, FeatureType, AlignerType>(*currMesh, alignerParam, cb);
                    return ok;
                }                
                default: assert(0);
            }   //end switch(ftype)
            return true;
        }   //end case AF_EXTRACTION
        case AF_MATCHING :
        {
            //set error message just once to avoid code grow long...
            errorMessage = QString("Probably the cause is one of the following:\n") +
                           QString("1 - Features have not been computed for these meshes.\n") +\
                           QString("2 - 'Number of neighboor picked' is > of 'Number of feature extracted from move mesh'.\n") +\
                           QString("3 - 'Number of base features' is > of 'Number of feature extracted from fix mesh'.\n") +
                           QString("4 - The base of feature selected doesn't pass inter-distance check.\n") +
                           QString("5 - No matches have been found.");

            switch(featureType)
            {                
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 3> FeatureType; //define needed typedef FeatureType                    
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<MeshType, AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    return MatchingOperation<MeshType, FeatureType, AlignerType>(*mFix, *mMov, alignerParam, cb);
                }
                case 1:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<MeshType, AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    return MatchingOperation<MeshType, FeatureType, AlignerType>(*mFix, *mMov, alignerParam, cb);
                }                
                default: assert(0);
            }  //end switch(ftype)
            return false;
        }  //end case AF_MATCHING
        case AF_RIGID_TRANSFORMATION :
        {          
            //fill an array of error messages. this avoid the code grow long...
            QString errMessages[4] = {"","Features have not been computed for these meshes.\n","Something wrong during matching!\nProbably a not enough sparse base has been picked.","Error: can't find a rigid transformation!"};

            switch(featureType)
            {                
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<MeshType, AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    int exitCode = RigidTransformationOperation<MeshType, FeatureType, AlignerType>(*mFix, *mMov, alignerParam, cb);
                    if(exitCode){ errorMessage = errMessages[exitCode]; return false; }
                    return true;
                }
                case 1:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<MeshType, AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    int exitCode = RigidTransformationOperation<MeshType, FeatureType, AlignerType>(*mFix, *mMov, alignerParam, cb);
                    if(exitCode){ errorMessage = errMessages[exitCode]; return false; }
                    return true;
                }               
                default: assert(0);
            }  //end switch(ftype)
            return false;
        }  //end case AF_RIGID_TRANSFORMATION
        case AF_CONSENSUS :
        {            
            switch(featureType)
            {
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<MeshType, AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    int consensus = ConsensusOperation<MeshType, FeatureType, AlignerType>(*mFix, *mMov, alignerParam, cb);
                    //Log results
                    Log(GLLogStream::FILTER,"Consensus of %.2f%% (%i/%i vertices).",(float(consensus)/alignerParam.fullConsensusSamples)*100.0f, consensus, alignerParam.fullConsensusSamples);
                    return true;
                }
                case 1:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<MeshType, AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    int consensus = ConsensusOperation<MeshType, FeatureType, AlignerType>(*mFix, *mMov, alignerParam, cb);
                    //Log results
                    Log(GLLogStream::FILTER,"Consensus of %.2f%% (%i/%i vertices).",(float(consensus)/alignerParam.fullConsensusSamples)*100.0f, consensus, alignerParam.fullConsensusSamples);
                    return true;
                }
                default: assert(0);
            }
        }
        case AF_RANSAC:
        {
            //set error message just once to avoid code grow long...
            errorMessage = QString("One or more parameters are wrong.\n") +
                           QString("1 - Features have not been computed for these meshes.\n") +\
                           QString("2 - 'Number of neighboor picked' must be > 1.\n");           

            switch(featureType){
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<MeshType, AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    return RansacOperation<MeshType, FeatureType, AlignerType>(*mFix, *mMov, alignerParam, cb);
                }               
                case 1:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<MeshType, AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    return RansacOperation<MeshType, FeatureType, AlignerType>(*mFix, *mMov, alignerParam, cb);
                }                
                default: assert(0);
            }  // end switch(ftype)
            return true;
        }  //end case AF_RANSAC
        case AF_RANSAC_DIAGRAM:
        {                                    
            //set error message just once to avoid code grow long...
            errorMessage = QString("One or more parameters are wrong.\n") +
                           QString("1 - Features have not been computed for these meshes.\n") +\
                           QString("2 - 'Number of neighboor picked' must be > 1.\n");            

            switch(featureType){
                case 0:{
                    typedef SmoothCurvatureFeature<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<MeshType, AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    return RansacDiagramOperation<MeshType, FeatureType, AlignerType>(*mFix, *mMov, alignerParam, trials, from, to, step, cb);
                }
                case 1:{
                    typedef FeatureRGB<MeshType, 3> FeatureType; //define needed typedef FeatureType
                    typedef FeatureAlignment<MeshType, FeatureType> AlignerType;  //define the Aligner class
                    AlignerType::Parameters alignerParam(mFix->cm, mMov->cm);
                    setAlignmentParameters<MeshType, AlignerType>(mFix->cm, mMov->cm, par, alignerParam);
                    return RansacDiagramOperation<MeshType, FeatureType, AlignerType>(*mFix, *mMov, alignerParam, trials, from, to, step, cb);
                }                
                default: assert(0);
            }  // end switch(ftype)
            return true;
        }  //end case AF_RANSAC_DIAGRAM              
        case AF_PERLIN_COLOR:
        {
            //get current mesh from document
            MeshModel* m = md.mm();
            //read parameters
            float freq = par.getDynamicFloat("freq");//default frequency; grant to be the same for all mesh in the document            
            m->updateDataMask(MeshModel::MM_VERTCOLOR);  //make sure color per vertex is enabled

            tri::UpdateColor<MeshType>::PerlinColor(m->cm, md.bbox(), freq, Point3i(0,64,128));
            return true;
        }
        case AF_COLOR_NOISE:
        {
            //get current mesh from document
            MeshModel* m = md.mm();
            //read parameters
            int noiseBits = par.getInt("noiseBits");

            m->updateDataMask(MeshModel::MM_VERTCOLOR);  //make sure color per vertex is enabled

            tri::UpdateColor<MeshType>::ColorNoise(m->cm, noiseBits);
            return true;
        }                        
        default: assert(0);
    } // end switch(ID(filter))
    return false;
}

///--------------------------------------------------------------------------///
/// PLUGIN WRAPPERS                                                          ///
///--------------------------------------------------------------------------///

template<class MESH_TYPE, class FEATURE_TYPE> bool FilterFeatureAlignment::ComputeFeatureOperation(MeshModel& m, typename FEATURE_TYPE::Parameters& param, CallBackPos *cb)
{
    typedef MESH_TYPE MeshType;
    typedef FEATURE_TYPE FeatureType;

    //enables needed attributes
    m.updateDataMask(FeatureType::getRequirements());    

    //compute feature for the given mesh
    return FeatureType::ComputeFeature(m.cm, param, cb);
}

template<class MESH_TYPE, class FEATURE_TYPE, class ALIGNER_TYPE>
bool FilterFeatureAlignment::ExtractionOperation(MeshModel& m, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb)
{
    typedef MESH_TYPE MeshType;
    typedef FEATURE_TYPE FeatureType;    

    //enables needed attributes
    m.updateDataMask(FeatureType::getRequirements());
    //extract features
    vector<FeatureType*>* vecF = FeatureAlignment<MeshType,FeatureType>::extractFeatures(param.numFixFeatureSelected, m.cm, param.samplingStrategy, cb);
    if(!vecF) return false;  //something wrong!

    //clear old picked points, then, if requested, add all new points
    FeatureAlignment<MeshType,FeatureType>::ClearPickedPoints(m.cm);
    if(param.pickPoints) FeatureAlignment<MeshType,FeatureType>::AddPickedPoints(m.cm, *vecF);      //add points

    //clean up
    vecF->clear(); delete vecF; vecF = NULL;

    return true;
}

template<class MESH_TYPE, class FEATURE_TYPE, class ALIGNER_TYPE>
bool FilterFeatureAlignment::MatchingOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb)
{
    typedef MESH_TYPE MeshType;
    typedef FEATURE_TYPE FeatureType;
    typedef ALIGNER_TYPE AlignerType;

    //create vectors to hold tuples of bases and matches
    vector<FeatureType**>* baseVec = new vector<FeatureType**>();
    vector<FeatureType**>* matchesVec = new vector<FeatureType**>();

    AlignerType aligner;
    aligner.init(mFix.cm, mMov.cm, param);

    //execute matching procedure with requested parameters;    
    bool ok = FeatureAlignment<MeshType,FeatureType>::Matching(*(aligner.vecFFix), *(aligner.vecFMov), aligner.fkdTree, *baseVec, *matchesVec, param, cb);

    aligner.finalize();

    //write result in the log
    if(ok) mylogger("%i matches found.",matchesVec->size());

    //cleaning baseVec and matchesVec...
    FeatureAlignment<MeshType,FeatureType>::CleanTuplesVector(baseVec, true);
    FeatureAlignment<MeshType,FeatureType>::CleanTuplesVector(matchesVec, true);

    return ok;
}

template<class MESH_TYPE, class FEATURE_TYPE, class ALIGNER_TYPE>
int FilterFeatureAlignment::RigidTransformationOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb)
{
    typedef MESH_TYPE MeshType;
    typedef FEATURE_TYPE FeatureType;
    typedef ALIGNER_TYPE AlignerType;
    typedef typename MeshType::ScalarType ScalarType;
    typedef Matrix44<ScalarType> Matrix44Type;

    //create vectors to hold tuples of bases and matches
    vector<FeatureType**>* baseVec = new vector<FeatureType**>();
    vector<FeatureType**>* matchesVec = new vector<FeatureType**>();

    AlignerType aligner;
    aligner.init(mFix.cm, mMov.cm, param);

    //execute matching procedure with requested parameters;    
    bool ok = FeatureAlignment<MeshType,FeatureType>::Matching(*(aligner.vecFFix), *(aligner.vecFMov), aligner.fkdTree, *baseVec, *matchesVec, param, cb);
    if(!ok) return 2;   //exit code 2

    assert(baseVec->size()==1);  //now baseVec must hold exactly one base of features

    for(unsigned int j=0; j<matchesVec->size(); j++)
    {
        Matrix44Type tr;
        ok = FeatureAlignment<MeshType,FeatureType>::FindRigidTransformation(mFix.cm, mMov.cm, (*baseVec)[0], (*matchesVec)[j], param.nBase, tr, cb);
        if(!ok) return 3;  //exit code 3
    }

    //cleaning baseVec and matchesVec...
    FeatureAlignment<MeshType,FeatureType>::CleanTuplesVector(baseVec, true);
    FeatureAlignment<MeshType,FeatureType>::CleanTuplesVector(matchesVec, true);

    return 0; //all right
}

template<class MESH_TYPE, class FEATURE_TYPE, class ALIGNER_TYPE>
int FilterFeatureAlignment::ConsensusOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb)
{
    typedef MESH_TYPE MeshType;
    typedef FEATURE_TYPE FeatureType;
    typedef ALIGNER_TYPE AlignerType;
    typedef typename MeshType::ScalarType ScalarType;
    typedef typename MeshType::VertexType VertexType;
    typedef GridStaticPtr<VertexType,ScalarType> MeshGrid;
    typedef tri::VertTmark<MeshType> MarkerVertex;    

    //enables needed attributes. These are used by the getClosest functor.
    mFix.updateDataMask(MeshModel::MM_VERTMARK);
    mMov.updateDataMask(MeshModel::MM_VERTMARK);    

    AlignerType aligner;
    aligner.init(mFix.cm, mMov.cm, param);
    param.consOffset = 0.0f;
    int consensus = FeatureAlignment<MeshType,FeatureType>::Consensus(mFix.cm, mMov.cm, *(aligner.gridFix), aligner.markerFunctorFix, aligner.normBuckets, param, 0, cb);
    aligner.finalize();

    //return the consensus score
    return consensus;
}

template<class MESH_TYPE, class FEATURE_TYPE, class ALIGNER_TYPE>
bool FilterFeatureAlignment::RansacOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb)
{
    typedef MESH_TYPE MeshType;
    typedef FEATURE_TYPE FeatureType;
    typedef ALIGNER_TYPE AlignerType;
    typedef typename MeshType::ScalarType ScalarType;
    typedef Matrix44<ScalarType> Matrix44Type;


    //enables needed attributes. MM_VERTMARK is used by the getClosest functor.
    mFix.updateDataMask(MeshModel::MM_VERTMARK);
    mMov.updateDataMask(MeshModel::MM_VERTMARK);

    AlignerType aligner;
    aligner.init(mFix.cm, mMov.cm, param);

    //perform RANSAC and get best transformation matrix
    Matrix44Type tr = aligner.align(mFix.cm, mMov.cm, param, cb);

    aligner.finalize();

    //apply transformation. If ransac don't find a good matrix, identity is returned; so nothing is wrong here...
    mMov.cm.Tr = tr * mMov.cm.Tr;

    return true;  //all right
}

template<class MESH_TYPE, class FEATURE_TYPE, class ALIGNER_TYPE>
bool FilterFeatureAlignment::RansacDiagramOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, int trials, int from, int to, int step, CallBackPos *cb)
{
    typedef MESH_TYPE MeshType;
    typedef FEATURE_TYPE FeatureType;
    typedef ALIGNER_TYPE AlignerType;
    typedef typename MeshType::ScalarType ScalarType;
    typedef Matrix44<ScalarType> Matrix44Type;

    //variables needed for progress bar callback
    float progBar = 0.0f;
    float offset = 100.0f/(trials);

    //enables needed attributes. MM_VERTMARK is used by the getClosest functor.
    mFix.updateDataMask(MeshModel::MM_VERTMARK);
    mMov.updateDataMask(MeshModel::MM_VERTMARK);

    FILE* file = fopen("RansacDiagram.txt","w+");
    fprintf(file,"FixMesh#%s\nMoveMesh#%s\nFeature#%s\nVertices#%i\nOverlap#%.2f%%\nShort threshold#%.2f%%#%.2f%% total#\nAcceptance#%.2f%%#%.2f%% total#\nTrials#%i\n",mFix.fileName.c_str(),mMov.fileName.c_str(),FeatureType::getName(),mFix.cm.VertexNumber(),param.overlap,param.shortConsOffset,(param.shortConsOffset*param.overlap/100.0f),param.consOffset,(param.consOffset*param.overlap/100.0f),trials); fflush(file);
    fprintf(file,"Iterations#Mean Time in Sec#Succ. Rate#FailRate/Sec\n0#0#0#0\n"); fflush(file);

    AlignerType aligner;
    float probSucc = 0.0f, meanTime = 0.0f, failPerSec = -1.0f;
    int counterSucc = 0, dif = 0; time_t start, end;
    param.counterSucc = &counterSucc;
    param.ransacIter = from;

    while(param.ransacIter<=to)
    {
        for(int i=0; i<trials; i++){
            //callback handling
            if(cb){ progBar+=offset; cb(int(progBar),"Computing diagram..."); }

            time(&start);  //start timer

            aligner.init(mFix.cm, mMov.cm, param);
            aligner.align(mFix.cm, mMov.cm, param);

            time(&end);  //stop timer
            dif += int(difftime(end,start));            
        }

        probSucc = counterSucc/float(trials);               //k = prob succ in 1 iteration
        meanTime = dif/float(trials);                       //t=sec elapsed to perform N ransac iterations
        failPerSec = std::pow(1-probSucc,float(trials)/dif);    //fail rate per sec is: (1-k)^(1/t)
        fprintf(file,"%i#%.2f#=100*%.2f#=100*%.2f\n", param.ransacIter, meanTime, probSucc, failPerSec); fflush(file);
        counterSucc = 0; dif = 0; progBar=0.0f;        
        param.ransacIter+=step;
    }

    fclose(file);
    return true;  //all right
}

Q_EXPORT_PLUGIN(FilterFeatureAlignment)
