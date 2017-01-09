#!/bin/bash

shopt -s extglob

[ ! -d "$(dirname "${BASH_SOURCE[0]}")/inc"    ] && ln -s ../gocd-base-ami/inc    "$(dirname "${BASH_SOURCE[0]}")/inc"
[ ! -d "$(dirname "${BASH_SOURCE[0]}")/target" ] && ln -s ../gocd-base-ami/target "$(dirname "${BASH_SOURCE[0]}")/target"

# shellcheck source=/dev/null
. "$(dirname "${BASH_SOURCE[0]}")/inc/commons.inc"

# shellcheck source=/dev/null
. "$(dirname "${BASH_SOURCE[0]}")/inc/keys.inc"



function createAgentCluster() {
  local stackname=$1

  vpcId=$(getDefaultVpcId)
  imageId=$(getImageId)
  dockerImage="$(cat "$(dirname "${BASH_SOURCE[0]}")"/target/gocdAgentImageName)"
  goServerUrl="$(cat "$(dirname "${BASH_SOURCE[0]}")"/target/goServerPublicIpAddress)"

  ecsAuth="$(echo -n "$(getDockerHubUsername):$(getDockerHubPassword)" | base64)"
  ecsEmail="$(getDockerHubEmail)"

  aws cloudformation create-stack \
  --stack-name "$stackname" \
  --template-body file://"$(dirname "${BASH_SOURCE[0]}")/agent.cf" \
  --parameters \
    ParameterKey=VpcId,ParameterValue="$vpcId" \
    ParameterKey=ImageId,ParameterValue="$imageId" \
    ParameterKey=DockerImage,ParameterValue="$dockerImage" \
    ParameterKey=EcsAuth,ParameterValue="$ecsAuth" \
    ParameterKey=EcsEmail,ParameterValue="$ecsEmail" \
    ParameterKey=GoServerUrl,ParameterValue="$goServerUrl" \
    ParameterKey=GithubPemKey,ParameterValue="$(getGithubPrivateKey)" \
  || return 1

  echo "*** please wait for create-stack to complete. this may take a few minutes."
  aws cloudformation wait stack-create-complete --stack-name "$stackname" --output text || return 2
}



function destroyAgentCluster() {
  local stackname=$1

  destroyStack "$stackname"  || return $?
}
