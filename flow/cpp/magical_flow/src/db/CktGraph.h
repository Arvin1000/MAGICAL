/**
 * @file CktGraph.h
 * @brief A graph for the implementation of circuit at one level of hierarchy
 * @author Keren Zhu, Mingjie Liu
 * @date 06/17/2019
 */

#ifndef MAGICAL_FLOW_CKTGRAPH_H_
#define MAGICAL_FLOW_CKTGRAPH_H_

#include "GraphComponents.h"
#include "parser/ParseGDS.h"
#include "Layout.h"
#include "TechDB.h"

PROJECT_NAMESPACE_BEGIN


/// @brief floorplan-related data structure
class FloorplanData
{
    public:
        /// @brief set boundary
        void setBoundary(LocType xLo, LocType yLo, LocType xHi, LocType yHi) 
        {
            _isBoundarySet = true;
            _boundary = Box<LocType>(xLo, yLo, xHi, yHi);
        }
        /// @brief clear the boundary
        void clearBoundary()
        {
            _isBoundarySet = false;
        }
        /// @brief get whether the boundary is set
        bool isBoundarySet() const { return _isBoundarySet; }
        /// @brief assign a net to left or right
        /// @param first: the name of the net
        /// @param second: 0: left 1: right -1 undefine
        void setNetAssignment(const std::string &netName, IntType assignStatus)
        {
            _isNetAssignmentSet = true;
            _netNameToAssignMap[netName] = assignStatus;
        }
        /// @brief clear the net IO pin assignment 
        void clearNetAssignment()
        {
            _isNetAssignmentSet = false;
        }
        /// @brief get whther the net IO pin assignmet is set
        bool isNetAssignmentSet() const { return _isNetAssignmentSet; }
        /// @brief get the status for one net
        /// @param net name
        /// @return 0->left 1->right -1> unset
        IntType netAssignment(const std::string &name)
        {
            if (_netNameToAssignMap.find(name) == _netNameToAssignMap.end())
            {
                return -1;
            }
            return _netNameToAssignMap.at(name);
        }
        
    private:
        Box<LocType> _boundary; ///< The boundary preset for the circuit
        bool _isBoundarySet = false; ///< Whether the boundary is set
        std::map<std::string, IntType> _netNameToAssignMap; ///< map[net name] = 0: left 1: right -1 undefine
        bool _isNetAssignmentSet = false; ///< Whether the net to IO pin assignment is configured
};

/// @class MAGICAL_FLOW::CktGraph
/// @brief each CktGraph represent a level of circuit in the hierarchical flow
class CktGraph
{
    public:
        /// @brief default construtor
        explicit CktGraph() = default; 
        void setTechDB(TechDB & techDB) { _techDB = techDB; }

        /// @brief backup the CktGraph
        void backup();
        /// @brief store the backuped CktGraph
        void restore();

        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the array of circuit nodes
        /// @return the array of circuit nodes
        const std::vector<CktNode> &                                nodeArray() const                                   { return _nodeArray; }
        /// @brief get the array of circuit nodes
        /// @return the array of circuit nodes
        std::vector<CktNode> &                                      nodeArray()                                         { return _nodeArray; }
        /// @brief resize the number of nodes
        /// @param the number of nodes
        void resizeNodeArray(IndexType numNodes)
        {
            AssertMsg(numNodes <= _nodeArray.size(), "Try resize nodes from size %u to %u", _nodeArray.size(), numNodes);
            _nodeArray.resize(numNodes);
        }
        /// @brief get the number of nodes
        /// @return the number of nodes this graph has
        IndexType                                                   numNodes() const                                    { return _nodeArray.size(); }
        /// @brief get a circuit node of this graph
        /// @param the index of node
        /// @return the circuit node
        //const CktNode &                                             node(IndexType nodeIdx) const                       { return _nodeArray.at(nodeIdx); }
        /// @brief get a circuit node of this graph
        /// @param the index of node
        /// @return the circuit node
        CktNode &                                                   node(IndexType nodeIdx)                             { return _nodeArray.at(nodeIdx); }
        /// @brief get the array of pins
        /// @return the array of pins
        const std::vector<Pin> &                                    pinArray() const                                    { return _pinArray; }
        /// @brief get the array of pins
        /// @return the array of pins
        std::vector<Pin> &                                          pinArray()                                          { return _pinArray; }
        /// @brief resize the number of pins
        /// @param the number of pins
        void resizePinArray(IndexType numPins)
        {
            AssertMsg(numPins <= _pinArray.size(), "Try resize pins from size %u to %u", _pinArray.size(), numPins);
            _pinArray.resize(numPins);
        }
        /// @brief get the number of pins
        /// @return the number of pins
        IndexType                                                   numPins() const                                     { return _pinArray.size(); }
        /// @brief get the number psub nets
        /// @return the number of psub nets
        IndexType                                                   numPsubs() const                                    { return _psubIdxArray.size(); }
        /// @brief get the number psub nets
        /// @return the number of psub nets
        IndexType                                                   numNwells() const                                   { return _nwellIdxArray.size(); }
        /// @brief get a pin of this graph
        /// @param the index of the pin of this graph
        /// @return the pin object
        //const Pin &                                                 pin(IndexType pinIdx) const                         { return _pinArray.at(pinIdx); }
        /// @brief get a pin of this graph
        /// @param the index of the pin of this graph
        /// @return the pin object
        Pin &                                                       pin(IndexType pinIdx)                               { return _pinArray.at(pinIdx); }
        /// @brief get the array of nets of this graph
        /// @return the array of nets of this graph
        const std::vector<Net> &                                    netArray() const                                    { return _netArray; }
        /// @brief get the array of nets of this graph
        /// @return the array of nets of this graph
        std::vector<Net> &                                          netArray()                                          { return _netArray; }
        /// @brief resize the number of nets
        /// @param the number of nets
        void resizeNetArray(IndexType numNets)
        {
            AssertMsg(numNets <= _netArray.size(), "Try resize nets from size %u to %u", _netArray.size(), numNets);
            _netArray.resize(numNets);
        }
        /// @brief get the number of nets this graph contains
        /// @return the number of nets this graph contains
        IndexType                                                   numNets() const                                     { return _netArray.size(); }
        /// @brief get a net of this graph
        /// @param the index of net in this graph
        /// @return a net
        //const Net &                                                 net(IndexType netIdx) const                         { return _netArray.at(netIdx); }
        /// @brief get a net of this graph
        /// @param the index of net in this graph
        /// @return a net
        Net &                                                       net(IndexType netIdx)                               { return _netArray.at(netIdx); }
        /// @brief get the net of a psub according to psubIdx
        /// @param the index of a psub net
        /// @return a net
        Net &                                                       psub(IndexType psubIdx)                             { return _netArray.at(_psubIdxArray.at(psubIdx)); }
        /// @brief get the net of a nwell according to nwellIdx
        /// @param the index of a nwell net
        /// @return a net
        Net &                                                       nwell(IndexType nwellIdx)                            { return _netArray.at(_nwellIdxArray.at(nwellIdx)); }
        /// @brief get the name of this circuit graph
        /// @return the name of this circuit
        const std::string &                                         name() const                                        { return _name; }
        /// @brief set the name of this circuit
        /// @param the name of this circuit
        void                                                        setName(const std::string &name)                    { _name = name; _refName = name; }
        /// @brief return the refname of this circuit
        /// @return the reference name of this circuit
        const std::string & refName() const { return _refName; }
        void setRefName(const std::string &refName) { _refName = refName; }
        /// @brief get the layout of this circuit
        /// @param the layout implementation of this circuit
        Layout &                                                    layout()                                            { return _layout; }
        /// @brief get the layout of this circuit
        /// @param the layout implementation of this circuit
        const Layout &                                              layoutConst() const                                 { return _layout; }
        /// @brief get the implementation type of this circuit
        /// @return the implementation type of this circuit
        ImplType implType() const { return _implType; }
        /// @brief set the implementation type of this circuit
        /// @param the implementation type of this circuit
        void setImplType(ImplType implType) { _implType = implType; }
        /// @brief get the index of implementation type configuration in the database
        /// @return the index of implementation type configuration in the database
        IndexType implIdx() const { return _implIdx; }
        /// @brief set the index of implementation type configuration in the database
        /// @param the index of implementation type configuration in the database
        void setImplIdx(IndexType implIdx) { _implIdx = implIdx; }
        /// @brief get GdsData 
        /// @return GdsData reference
        GdsData & gdsData() { return _gdsData; }
        /// @brief is Net Io shape has been flipped vertically
        /// @return boolean
        bool flipVertFlag() const { return _flipVertFlag; }
        /// @brief get the floorplan data 
        /// @return the floorplan solution data
        FloorplanData & fpData() { return _fpData; }

        /*------------------------------*/ 
        /* Vector operation             */
        /*------------------------------*/ 
        /// @brief allocate a new node
        /// @return the index of the new node
        IndexType allocateNode() { _nodeArray.emplace_back(CktNode()); return _nodeArray.size() - 1;}
        /// @brief allocate a new pin
        /// @return the index of a new pin
        IndexType allocatePin() { _pinArray.emplace_back(Pin()); return _pinArray.size() - 1; }
        /// @brief allocate a new net
        /// @return the index of a new net
        IndexType allocateNet() { _netArray.emplace_back(Net()); return _netArray.size() - 1; }
        /// @brief create a new substrate net
        /// @return the index of a new psub net
        IndexType allocatePsub() { IndexType netIdx = allocateNet(); _psubIdxArray.push_back(netIdx); return netIdx; }
        /// @brief add a existing net index as psub net
        /// @param self.designDB.numCthe net index of exising net
        void addPsubIdx(IndexType netIdx) { _psubIdxArray.push_back(netIdx); }
        /// @brief create a new nwell net
        /// @return the index of a new nwell net
        IndexType allocateNwell() { IndexType netIdx = allocateNet(); _nwellIdxArray.push_back(netIdx); return netIdx; }
        /// @brief add a existing net index as nwell net
        /// @param the net index of exising net
        void addNwellIdx(IndexType netIdx) { _nwellIdxArray.push_back(netIdx); }
        bool isImpl() const { return _isImplemented; }
        void setIsImpl(bool impl) { _isImplemented = impl; }
        /// @brief readin GDSII file into _layout
        /// @param GDSII filename
        void parseGDS(const std::string & fileName) { Parser parse(fileName, _layout, _techDB); }

        /*------------------------------*/ 
        /* Integration                  */
        /*------------------------------*/ 
        /// @brief flip all net Io shape according to vertical axis
        /// @param symmetry vertical axis x=axis
        void flipVert(LocType axis)
        {
            _flipVertFlag = !_flipVertFlag;
            for(Net &net : _netArray)
            {
                net.flipVert(axis);
            }
        }
        

    private:
        // Only for backup purpose
        struct CktGraphBackup
        {
            std::vector<CktNode> nodeArray;
            std::vector<Pin> pinArray;
            std::vector<Net> netArray;
            std::vector<IndexType> psubIdxArray;
            std::vector<IndexType> nwellIdxArray;
            Layout layout;
            bool isImplemented;
            bool flipVertFlag;
            GdsData gdsData;

        } _backup;
    private:
        TechDB _techDB;
        std::vector<CktNode> _nodeArray; ///< The circuit nodes of this graph
        std::vector<Pin> _pinArray; ///< The pins of the circuit
        std::vector<Net> _netArray; ///< The nets of the circuit
        std::vector<IndexType> _psubIdxArray; ///< The index of substrate nets in _netArray
        std::vector<IndexType> _nwellIdxArray; ///< The index of nwell nets in _netArray
        std::string _name = ""; ///< The name of this circuit
        std::string _refName = ""; ///< The ref name of this circuit
        Layout _layout; ///< The layout implementation for this circuit
        ImplType _implType = ImplType::UNSET; ///< The implementation set of this circuit
        IndexType _implIdx = INDEX_TYPE_MAX; ///< The index of this implementation type configuration in the database
        bool _isImplemented = false; 
        bool _flipVertFlag = false; ///< Flag indicating that net Io shape has been flipped vertically
        FloorplanData _fpData; ///< The data for floorplan solution
        /*------------------------------*/ 
        /* Integration                  */
        /*------------------------------*/ 
        GdsData _gdsData; ///< The gds data

};


inline void CktGraph::backup()
{
    _backup.nodeArray = this->_nodeArray;
    _backup.pinArray = this->_pinArray;
    _backup.netArray = this->_netArray;
    _backup.psubIdxArray = this->_psubIdxArray;
    _backup.nwellIdxArray = this->_nwellIdxArray;
    _backup.layout = this->_layout;
    _backup.isImplemented = this->_isImplemented;
    _backup.flipVertFlag = this->_flipVertFlag;
    _backup.gdsData = this->_gdsData;
}

inline void CktGraph::restore() 
{
    std::swap(_nodeArray, _backup.nodeArray);
    std::swap(_pinArray, _backup.pinArray);
    std::swap(_netArray, _backup.netArray);
    std::swap(_psubIdxArray, _backup.psubIdxArray);
    std::swap(_nwellIdxArray, _backup.nwellIdxArray);
    std::swap(_layout, _backup.layout);
    std::swap(_isImplemented, _backup.isImplemented);
    std::swap(_flipVertFlag, _backup.flipVertFlag);
    std::swap(_gdsData, _backup.gdsData);
    Assert(_layout.boundary().xLo() == LOC_TYPE_MAX);

}

PROJECT_NAMESPACE_END

#endif //MAGICAL_FLOW_CKTGRAPH_H_
