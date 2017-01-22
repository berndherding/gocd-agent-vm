#!/usr/bin/env bash

shopt -s extglob

[ ! -d "$(dirname "${BASH_SOURCE[0]}")/inc"    ] && ln -s ../gocd-base-ami/inc    "$(dirname "${BASH_SOURCE[0]}")/inc"
[ ! -d "$(dirname "${BASH_SOURCE[0]}")/target" ] && ln -s ../gocd-base-ami/target "$(dirname "${BASH_SOURCE[0]}")/target"

# shellcheck source=/dev/null
. "$(dirname "${BASH_SOURCE[0]}")/inc/commons.inc"

# shellcheck source=/dev/null
. "$(dirname "${BASH_SOURCE[0]}")/inc/keys.inc"

ERROR_createAgentCluster_createStack=1
ERROR_createAgentCluster_waitStack=2
ERROR_noDockerHubUsername=3
ERROR_noDockerHubPassword=4
ERROR_noDockerHubEmail=5



function createAgentCluster() {
  local label=$1

  [ -z "$DOCKER_HUB_USERNAME" ] && return $ERROR_noDockerHubUsername
  [ -z "$DOCKER_HUB_PASSWORD" ] && return $ERROR_noDockerHubPassword
  [ -z "$DOCKER_HUB_EMAIL" ]    && return $ERROR_noDockerHubEmail

  gocd_avm="$(getStackname "gocd-avm" "$label")"

  vpcId=$(getDefaultVpcId)
  imageId=$(getImageId)
  dockerImage="$(cat "$(dirname "${BASH_SOURCE[0]}")"/target/gocdAgentImageName)"
  goServerUrl="$(cat "$(dirname "${BASH_SOURCE[0]}")"/target/goServerPublicIpAddress)"

  ecsAuth="$(echo -n "$DOCKER_HUB_USERNAME:$DOCKER_HUB_PASSWORD" | base64)"
  ecsEmail="$DOCKER_HUB_EMAIL"

  aws cloudformation create-stack \
  --stack-name "$gocd_avm" \
  --template-body file://"$(dirname "${BASH_SOURCE[0]}")/agent.cf" \
  --parameters \
    ParameterKey=VpcId,ParameterValue="$vpcId" \
    ParameterKey=ImageId,ParameterValue="$imageId" \
    ParameterKey=DockerImage,ParameterValue="$dockerImage" \
    ParameterKey=EcsAuth,ParameterValue="$ecsAuth" \
    ParameterKey=EcsEmail,ParameterValue="$ecsEmail" \
    ParameterKey=GoServerUrl,ParameterValue="$goServerUrl" \
    ParameterKey=GithubPrivateKey,ParameterValue="$(getGithubPrivateKey)" \
  || return $ERROR_createAgentCluster_createStack

  echo "*** please wait for create-stack to complete. this may take a few minutes."
  aws cloudformation wait stack-create-complete --stack-name "$gocd_avm" --output text \
  || return $ERROR_createAgentCluster_waitStack
}



function destroyAgentCluster() {
  local label=$1
  gocd_avm="$(getStackname "gocd-avm" "$label")"
  destroyStack "$gocd_avm" || return $?
}
