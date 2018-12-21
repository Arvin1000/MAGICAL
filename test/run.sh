##
# @file   run.sh
# @author Yibo Lin
# @date   Dec 2018
#
#!/bin/bash

if [ ! -n "$1" ]
then
  echo "Usage: `basename $0` circuit techfile spacing_rule width_area_rule enclosure_rule well_contact_GDSII"
  return 
fi  

TOP_CIRCUIT=$1
TOP_CIRCUIT_NAME=$(basename ${CIRCUIT})
TOP_TECHFILE=$2
TOP_SPACING_RULE_FILE=$3
TOP_WIDTH_AREA_RULE_FILE=$4
TOP_ENCLOSURE_RULE_FILE=$5
TOP_WELL_CON_GDS_FILE=$6

# get script directory instead of execution directory
TOP_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
TOP_RESULT_DIR=results

echo "CIRCUIT = ${TOP_CIRCUIT}"
echo "CIRCUIT_NAME = ${TOP_CIRCUIT_NAME}"
echo "TOP_SCRIPT_DIR = ${TOP_SCRIPT_DIR}"

## run constraint generation 
#echo "${TOP_CIRCUIT_NAME}: constraint generation"
#source ${TOP_SCRIPT_DIR}/../constraint_generation/test/run.sh ${TOP_CIRCUIT}/${TOP_CIRCUIT_NAME}.sp
#
## run device generation 
#echo "${TOP_CIRCUIT_NAME}: device generation"
#source ${TOP_SCRIPT_DIR}/../placement/device_generation/test/run.sh ${TOP_CIRCUIT}

# run analog placement 
echo "${TOP_CIRCUIT_NAME}: analog placement"
source ${TOP_SCRIPT_DIR}/../placement/analog_place/test/run.sh \
    ${TOP_RESULT_DIR}/${TOP_CIRCUIT} \
    ${TOP_TECHFILE} \
    ${TOP_SPACING_RULE_FILE} \
    ${TOP_WIDTH_AREA_RULE_FILE} 

# run well generation 
echo "${TOP_CIRCUIT_NAME}: well generation"
source ${TOP_SCRIPT_DIR}/../placement/well_generation/test/run.sh \
    ${TOP_RESULT_DIR}/${TOP_CIRCUIT} \
    ${TOP_TECHFILE} \
    ${TOP_SPACING_RULE_FILE} \
    ${TOP_ENCLOSURE_RULE_FILE} \
    ${TOP_WELL_CON_GDS_FILE} \
    ${TOP_RESULT_DIR}/${TOP_CIRCUIT}/result_legal_detail.txt
