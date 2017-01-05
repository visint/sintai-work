<?php
function is_legal_user($user, $passwd)

  // get user password(encrypted)
  $userpasswd = trim(shell_exec("sudo awk -F: '\$1 ~/^$user\$/ {print \$2}' /etc/shadow"));
  
  echo $userpasswd."\n";

  // get user password salt
  if (strncmp($userpasswd, '$1$', strlen('$1$')) == 0)
    {
      $salt = strrev(stristr(strrev($userpasswd), '$'));
    }
  else
    {
      $salt = substr ($userpasswd, 0, 2);
    }

  $encryptpasswd = trim(shell_exec("sudo ".CRYPTPRO." $passwd '$salt'"));

  if ($encryptpasswd == $userpasswd)
    {
      return true;
    }

  return false;
}

is_legal_user('root','123456');

?>
