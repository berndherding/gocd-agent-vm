#!/usr/bin/env bash

shopt -s extglob

[ ! -d "$(dirname "${BASH_SOURCE[0]}")/inc"    ] \
  && ln -s "$(dirname "${BASH_SOURCE[0]}")/../gocd-base-ami/inc" "$(dirname "${BASH_SOURCE[0]}")/inc"

[ ! -f "$(dirname "${BASH_SOURCE[0]}")/target/imageId" ] \
  && ln -s "$(dirname "${BASH_SOURCE[0]}")/../gocd-base-ami/target" "$(dirname "${BASH_SOURCE[0]}")/target"



# shellcheck source=/dev/null
. "$(dirname "${BASH_SOURCE[0]}")/inc/commons.inc"

# shellcheck source=/dev/null
. "$(dirname "${BASH_SOURCE[0]}")/inc/keys.inc"

ERROR_createAgentCluster_createStack=1
ERROR_createAgentCluster_waitStack=2
ERROR_noDockerHubUsername=3
ERROR_noDockerHubPassword=4
ERROR_noDockerHubEmail=5
ERROR_updateAgentCluster_updateStack=6
ERROR_updateAgentCluster_waitStack=7
ERROR_createAgentSwarm_createStack=8
ERROR_createAgentSwarm_waitStack=9



function createAgentSwarm() {
  local env=$1

  stack="$(getStackname "avm" "$env")"

  release=$(getRelease)
  vpcId=$(getDefaultVpcId)
  imageId=$(getImageId)

  gocdAgentImage="$(cat "$(dirname "${BASH_SOURCE[0]}")"/target/gocdAgentImageName 2> /dev/null)"
  [ -n "$GOCD_AGENT_IMAGE_TAG" ] && gocdAgentImage="$DOCKER_HUB_USERNAME/gocd-agent:$GOCD_AGENT_IMAGE_TAG"

  aws cloudformation create-stack \
  --stack-name "$stack" \
  --template-body file://"$(dirname "${BASH_SOURCE[0]}")/swarm.cf" \
  --parameters \
    ParameterKey=Release,ParameterValue="$release" \
    ParameterKey=VpcId,ParameterValue="$vpcId"     \
    ParameterKey=ImageId,ParameterValue="$imageId" \
    ParameterKey=DockerImage,ParameterValue="$gocdAgentImage" \
    ParameterKey=GoServerUrl,ParameterValue="$goServerUrl" \
    ParameterKey=GithubPrivateKey,ParameterValue="$(getGithubPrivateKey)" \
  || return $ERROR_createAgentSwarm_createStack

  echo "*** please wait for create-stack to complete. this may take a few minutes."
  aws cloudformation wait stack-create-complete --stack-name "$stack" --output text \
  || return $ERROR_createAgentSwarm_waitStack
}



function createAgentCluster() {
  local env=$1
  local instanceType=$2

  [ -z "$DOCKER_HUB_USERNAME" ] && return $ERROR_noDockerHubUsername
  [ -z "$DOCKER_HUB_PASSWORD" ] && return $ERROR_noDockerHubPassword
  [ -z "$DOCKER_HUB_EMAIL" ]    && return $ERROR_noDockerHubEmail

  stack="$(getStackname "avm" "$env")"

  echo "create agent cluster $stack"

  vpcId=$(getDefaultVpcId)
  release=$(getRelease)
  imageId=$(getImageId)
  ecsAuth="$(echo -n "$DOCKER_HUB_USERNAME:$DOCKER_HUB_PASSWORD" | base64)"
  ecsEmail="$DOCKER_HUB_EMAIL"

  echo "* release: $release"
  echo "* imageId: $imageId"

  echo "* creating new agent stack $stack"
  aws cloudformation create-stack \
  --stack-name "$stack" \
  --template-body file://"$(dirname "${BASH_SOURCE[0]}")/create-agent.cf" \
  --parameters \
    ParameterKey=VpcId,ParameterValue="$vpcId"       \
    ParameterKey=Release,ParameterValue="$release"   \
    ParameterKey=ImageId,ParameterValue="$imageId"   \
    ParameterKey=EcsAuth,ParameterValue="$ecsAuth"   \
    ParameterKey=EcsEmail,ParameterValue="$ecsEmail" \
    ParameterKey=InstanceType,ParameterValue="$instanceType" \
  > /dev/null || return $ERROR_createAgentCluster_createStack

  echo "* waiting for $stack to complete. this may take a few minutes."
  aws cloudformation wait stack-create-complete --stack-name "$stack" --output text \
  || return $ERROR_createAgentCluster_waitStack

  echo "$stack"        > "$(dirname "${BASH_SOURCE[0]}")/target/gocdAvm"
  echo "$release"      > "$(dirname "${BASH_SOURCE[0]}")/target/release"
  echo "$instanceType" > "$(dirname "${BASH_SOURCE[0]}")/target/instanceType"
}



function updateAgentCluster() {
  local env=$1

  stack="$(cat "$(dirname "${BASH_SOURCE[0]}")"/target/gocdAvm 2> /dev/null)"

  echo
  echo "update agent cluster $stack"

  vpcId=$(getDefaultVpcId)
  imageId=$(getImageId)
  ecsAuth="$(echo -n "$DOCKER_HUB_USERNAME:$DOCKER_HUB_PASSWORD" | base64)"
  ecsEmail="$DOCKER_HUB_EMAIL"

  release="$(cat "$(dirname "${BASH_SOURCE[0]}")/target/release" 2> /dev/null)"
  instanceType="$(cat "$(dirname "${BASH_SOURCE[0]}")"/target/instanceType 2> /dev/null)"

  gocdAgentImage="$(cat "$(dirname "${BASH_SOURCE[0]}")"/target/gocdAgentImageName 2> /dev/null)"
  [ -n "$GOCD_AGENT_IMAGE_TAG" ] && gocdAgentImage="$DOCKER_HUB_USERNAME/gocd-agent:$GOCD_AGENT_IMAGE_TAG"

  goServerUrl="$(cat "$(dirname "${BASH_SOURCE[0]}")"/target/publicIp)"
  [ -n "$GO_SERVER_URL" ] && goServerUrl="$GO_SERVER_URL"

  echo "* release: $release"
  echo "* imageId: $imageId"
  echo "* gocd agent image: $gocdAgentImage"
  echo "* go server url: $goServerUrl"

  "$(dirname "${BASH_SOURCE[0]}")/create_update_template.sh"

  echo "* updating agent stack $stack"
  aws cloudformation update-stack \
  --stack-name "$stack" \
  --template-body file://"$(dirname "${BASH_SOURCE[0]}")/update-agent.cf" \
  --parameters \
    ParameterKey=VpcId,ParameterValue="$vpcId"       \
    ParameterKey=Release,ParameterValue="$release"   \
    ParameterKey=ImageId,ParameterValue="$imageId"   \
    ParameterKey=EcsAuth,ParameterValue="$ecsAuth"   \
    ParameterKey=EcsEmail,ParameterValue="$ecsEmail" \
    ParameterKey=GoServerUrl,ParameterValue="$goServerUrl" \
    ParameterKey=InstanceType,ParameterValue="$instanceType"     \
    ParameterKey=GoCDAgentImage,ParameterValue="$gocdAgentImage" \
    ParameterKey=GithubPrivateKey,ParameterValue="$(getGithubPrivateKey)" \
  > /dev/null || return $ERROR_updateAgentCluster_updateStack

  echo "*** please wait for create-stack to complete. this may take a few minutes."
  aws cloudformation wait stack-update-complete --stack-name "$stack" --output text \
  || return $ERROR_updateAgentCluster_waitStack
}



function destroyAgentCluster() {
  local env=$1
  stack="$(getStackname "avm" "$env")"
  destroyStack "$stack" || return $?
}
