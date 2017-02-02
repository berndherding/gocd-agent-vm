#!/usr/bin/env bash

(
  awk '/"Resources" *: *{/ {print; p=1; next} !p' create-agent.cf 
  cat update.cf 
  awk '/"Resources" *: *{/ {       p=1; next}  p' create-agent.cf
) > "$(dirname "${BASH_SOURCE[0]}")/update-agent.cf"
