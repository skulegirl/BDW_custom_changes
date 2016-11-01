<?php

/**
 * Custom function to see if a specific file has been downloaded yet by the
 * current user.
 * This replaces all the functions below, since they use ubercart, and this
 * just uses a plain file download, supported by download_count module.
 */
function BDW_custom_changes_FileHasBeenDownloaded ($filename) {
  global $user;

  $result = db_query('SELECT COUNT(dc.dcid) AS count FROM {download_count} dc JOIN {files} f ON dc.fid = f.fid WHERE f.filename = :filename AND dc.uid = :uid', array(':filename' => $filename, ':uid' => $user->uid));

  foreach ($result as $download) {
    $count = $download->count;
    break;
  }

  if ($count)
  {
    return TRUE;
  }
  return FALSE;
}

/** 
 * Custom function to get full list of files that are available but have
 * not yet been acquired (i.e. purchased).
 */
function BDW_custom_changes_GetAvailableUnacquiredFiles () {
  global $user;
  
  $files = BDW_custoom_changes_GetAvailableFiles();
  foreach ($files as &$file) {
    $file['acquired'] = FALSE;
  }

// NOTE - this complicated logic and the GetAvailableFiles function should just be replaced with an JOIN.

  $file_downloads = db_query('SELECT * FROM {uc_file_users} AS ufu INNER JOIN {uc_files} AS uf ON ufu.fid = uf.fid WHERE ufu.uid = :uid ORDER BY uf.filename ASC', array(':uid' =>$user->uid));
  foreach ($file_downloads as $file_download) {
    foreach ($files as &$file) {
      if (strcmp($file['filename'], $file_download->filename) == 0)
      {
	$file['acquired'] = TRUE;
        break;
      }
    }
  }
  return $files;
}

/**
 * Custom function to get list of files that have been acquired but not
 * yet downloaded
 */
function BDW_custom_changes_GetFilesForDownload()
{
  global $user;

  $files=array();
  $file_downloads = db_query('SELECT * FROM {uc_file_users} AS ufu INNER JOIN {uc_files} AS uf ON ufu.fid = uf.fid INNER JOIN uc_file_products as p ON p.pfid = ufu.pfid WHERE ufu.uid = :uid ORDER BY uf.filename ASC', array(':uid' =>$user->uid));
  foreach ($file_downloads as $file_download) {
       $files[] = array( 'filename' => $file_download->filename,
       			 'fid' => $file_download->fid,
			 'file_key' => $file_download->file_key,
			 'accessed' => $file_download->accessed,
			 'description' => $file_download->description);
  }

  return $files;
}

/** 
 * Custom function to get list of available files for the user, based on 
 * permissions
 */
function BDW_custoom_changes_GetAvailableFiles()
{
  global $user;

  $files=array();
  if (in_array('Founding Member with BDA', array_values($user->roles))) {
    $files[] = array( 'filename' => 'NewMexicoFrugalShunpiker.pdf',
    		      'url' => '/content/frugal-shunpikers-guide-rv-boondocking-new-mexico',
		      'title' => "Frugal Shunpiker's Guide to RV Boondocking in New Mexico");
  }
  return $files;
}

/**
 ** Custom function to get number of boondocking locations
 **/
function BDW_custom_changes_GetBDACount()
{
  return db_query("SELECT COUNT(*) FROM {flagging} INNER JOIN {users} ON flagging.entity_id=users.uid WHERE flagging.fid = 2 AND flagging.uid = 0")->fetchField();
}


function _BDW_get_services_icon($image, $alttext, $active = TRUE, $disptext = NULL, $class = NULL, $legend = FALSE)
{
  $output = "<div class='ServiceIcon";
  if (isset($class))
  {
    $output .= " $class";
  }
  if (!$active)
  {
    $output .= " NotAvailable";
    $alttext = "No " . $alttext;
  }
  $output .= "'>";
  $output .= "<span title='$alttext'>";
  $output .= "<img src='$image' alt='$alttext'/>";
  if (isset($disptext))
  {
    $output .= "<h3>$disptext</h3>";
  }
  $output .= "</span>";
  if ($legend) { $output .= "<span class='LegendText'>$alttext</span>"; }
  $output .= "</div>";
  
  return $output;
}

function _BDW_site_provides($vars)
{
  $basepath = '/sites/boondockerswelcome.com/files/media/iconsets/services/';

  $services = FALSE;
  $dump = $electric = $water = FALSE;
  $output = "<div class='SiteProvides'>";

  $active = FALSE;
  if (!empty($vars->field_ba_pullthrough_spot) && $vars->field_ba_pullthrough_spot['und'][0]['value'] === "Yes") {
    $active = TRUE;
    $services = TRUE;
  }
  $output .= _BDW_get_services_icon($basepath . 'pull-through-parking-icon.png', 'Pull Through Parking Available', $active);

  $active = FALSE;
  if (!empty($vars->field_ba_wifi_available) && $vars->field_ba_wifi_available['und'][0]['value'] === "Yes") {
    $active = TRUE;
    $services = TRUE;
  }
  $output .= _BDW_get_services_icon($basepath . 'wifi-icon.png', 'Wifi Available', $active);


  if(!empty($vars->field_ba_hookups))
  {
    foreach ($vars->field_ba_hookups['und'] as $hookup)
    {
      switch ($hookup['value']) {
        case 'Electric Hookup Available': 
          $electric = TRUE;
          $services = TRUE;
          break;
        case 'Water Hookup Available':
          $water = TRUE;
          $services = TRUE;
          break;
        case 'Dump Available':
          $dump = TRUE;
          $services = TRUE;
          break;
      }
    }
  }

  $output .= _BDW_get_services_icon($basepath . 'power-icon.png', 'Electric Hookup Available', $electric);
  $output .= _BDW_get_services_icon($basepath . 'water-icon.png', 'Water Hookup Available', $water);
  $output .= _BDW_get_services_icon($basepath . 'rv-dump-icon.png', 'Dump Available', $dump);

  if ($services === FALSE)
  {
    $output .= "<h2 class='NoServices'>No Services Provided</h2>";
  }
  $output .= "</div>";
  return $output;
}


function _BDW_site_welcomes($vars) 
{
  $basepath = '/sites/boondockerswelcome.com/files/media/iconsets/services/';

  $services = FALSE;
  $output = "<div class='SiteWelcomes'>";

  $active = FALSE;
  if ($vars->field_ba_slideout_awning['und'][0]['value'] === "Yes") {  
    $active = TRUE;
    $services = TRUE;
  }
  $output .= _BDW_get_services_icon($basepath . 'slideouts-icon.png', 'Slideouts And Awnings', $active);
  
  $active = FALSE;
  if ($vars->field_generators_allowed['und'][0]['value'] === "Yes") {  
    $active = TRUE;
    $services = TRUE;
  }
  $output .= _BDW_get_services_icon($basepath . 'generator-icon.png', 'Generators Allowed', $active);

  $active = FALSE;
  if ($vars->field_lawn_chairs_allowed['und'][0]['value'] === "Yes") {  
    $active = TRUE;
    $services = TRUE;
  }
  $output .= _BDW_get_services_icon($basepath . 'lawnchairs-icon.png', 'Lawn Chairs Allowed', $active);
  
  $active = FALSE;
  if ($vars->field_barbecues_allowed['und'][0]['value'] === "Yes") {  
    $active = TRUE;
    $services = TRUE;
  }
  $output .= _BDW_get_services_icon($basepath . 'bbq-icon.png', 'BBQs Allowed', $active);

  $active = FALSE;
  if ($vars->field_ba_pets_welcome['und'][0]['value'] === "Yes") {  
    $active = TRUE;
    $services = TRUE;
  }
  $output .= _BDW_get_services_icon($basepath . 'pets-icon.png', 'Pets Welcome', $active);


  if ($services === FALSE)
  {
    $output .= "<h2 class='NoServices'>Stealth Mode Please!</h2>";
  }
  $output .= "</div>";
  return $output;
}

function BDW_echo_BDA_services($data)
{
  echo "<div class='SiteServices'>";
  $output = _BDW_site_provides($data->_field_data['pid']['entity']);
  echo "<h2 class='block-title'>Site Has Available:</h2>";
  echo $output;

  $output = _BDW_site_welcomes($data->_field_data['pid']['entity']);
  echo "<h2 class='block-title'>Site Welcomes:</h2>";
  echo $output;

  echo "</div>";
}

function BDW_echo_BDA_details($data)
{
  $basepath = '/sites/boondockerswelcome.com/files/media/iconsets/services/';
  $vars = $data->_field_data['pid']['entity'];

  echo "<div class='SiteDetails'>";
  echo "<h2 class='block-title'>Site Details:</h2>";

  /* Number of Spots */
  $rvcount = $vars->field_ba_number_of_spots['und'][0]['value'];
  $alttext = "$rvcount spot" . ($rvcount > 1 ? 's' : '') . " available";
  echo _BDW_get_services_icon($basepath . 'rv-count-icon.png', $alttext, TRUE, $rvcount, 'RVCount');

  /* Max Size */
  $rvsize = $vars->field_ba_max_rv_length_int['und'][0]['value'];
  if ($rvsize < 100)
  {
    $alttext = "RV $rvsize feet max";
    $disptext = "&lt ". $rvsize;
  }
  else
  {
    $alttext = "any size RV"; 
    $disptext = "40+";
  }
  echo _BDW_get_services_icon($basepath . 'rv-size-icon.png', $alttext, TRUE, $disptext, 'RVSize');

  /* Number of Nights */
  $nightcount = $vars->field_ba_max_nights['und'][0]['value'];
  echo "<div class='MaxNights ServiceIcon'>";
  if ($nightcount === "we'll play it by ear")
  {
    $alttext = "We&#39;ll play it by ear";
    $disptext = "?";
  }
  else
  {
    $alttext = "$nightcount nights max";
    $disptext = "$nightcount";
  }
  echo _BDW_get_services_icon($basepath . 'night-count-icon.png', $alttext, TRUE, $disptext, 'MaxNights');

  echo "</div>";
}

function _BDW_get_BDA_legend() {
  $basepath = '/sites/boondockerswelcome.com/files/media/iconsets/services/';
  $output = '<p class="LegendHeader">If an icon is greyed out, then that service is not available / not allowed</p>';

  $output .= "<div class='SiteDetails SiteServicesLegend'>";
  $output .= _BDW_get_services_icon($basepath . 'rv-count-icon.png', 'Number of spots', TRUE, '2', 'RVCount LegendIcon', TRUE);
  $output .= _BDW_get_services_icon($basepath . 'night-count-icon.png', 'Maximum Night Stay', TRUE, '1', 'MaxNights LegendIcon', TRUE);
  $output .= _BDW_get_services_icon($basepath . 'rv-size-icon.png', 'Maximum RV Size in Feet', TRUE, '20', 'RVSize LegendIcon', TRUE);
  $output .= _BDW_get_services_icon($basepath . 'pull-through-parking-icon.png', 'Pull Through Parking Available', TRUE, NULL, 'LegendIcon', TRUE);
  $output .= _BDW_get_services_icon($basepath . 'wifi-icon.png', 'Wifi Available', TRUE, NULL, 'LegendIcon', TRUE);
  $output .= _BDW_get_services_icon($basepath . 'power-icon.png', 'Electric Hookup Available', TRUE, NULL, 'LegendIcon', TRUE);
  $output .= _BDW_get_services_icon($basepath . 'water-icon.png', 'Water Hookup Available', TRUE, NULL, 'LegendIcon', TRUE);
  $output .= _BDW_get_services_icon($basepath . 'rv-dump-icon.png', 'Dump Available', TRUE, NULL, 'LegendIcon', TRUE);
  $output .= _BDW_get_services_icon($basepath . 'slideouts-icon.png', 'Slideouts /Awnings Allowed', TRUE, NULL, 'LegendIcon', TRUE);
  $output .= _BDW_get_services_icon($basepath . 'generator-icon.png', 'Generators Allowed', TRUE, NULL, 'LegendIcon', TRUE);
  $output .= _BDW_get_services_icon($basepath . 'lawnchairs-icon.png', 'Lawn Chairs Allowed', TRUE, NULL, 'LegendIcon', TRUE);
  $output .= _BDW_get_services_icon($basepath . 'bbq-icon.png', 'BBQs Allowed', TRUE, NULL, 'LegendIcon', TRUE);
  $output .= _BDW_get_services_icon($basepath . 'pets-icon.png', 'Pets Welcome', TRUE, NULL, 'LegendIcon', TRUE);

  $output .= "</div>";
  $output .= "</div>";
  return $output;
}

function BDW_echo_BDA_legend()
{
  $attributes = array(
    'text' => _BDW_get_BDA_legend(),
    'title' => 'Site Icon Legend',
    'style' => 'McPopup',
    'id' => 'legend',
    'class' => 'legend',
    'width' => '280',
    'close' => 'TRUE',
  );
  module_load_include('inc', 'popup', 'includes/popup.api');
  $retval = popup($attributes);
  echo $retval;
}

function _BDW_get_memberships()
{
  return array(
    'escapee_member' => array('escapee_member_icon.png', 'Escapee Member', 'SKP Number'),
    'rvillage_member' => array('rvillage_member_icon.png', 'RVillage Member', 'RVillage Username'),
    'fmca_member' => array('fmca_member_icon.png', 'FMCA Member', 'FMCA Membership Number'),
  );
}

function BDW_echo_group_memberships($data)
{
  $basepath = '/media/iconsets/groupbadges/';
  $vars = $data->_field_data['pid']['entity'];

  $output =  "<div class='GroupMemberships'>";

  global $user;
  if (in_array('full member', array_values($user->roles))) {
    //$display_data = TRUE; // temporarily remove until I figure out what's broken
    $display_data = FALSE;
  }
  else
  {
    $display_data = FALSE;
  }

  $memberships = _BDW_get_memberships();
  foreach ($memberships as $membership => $details)
  {
    $fieldname = 'field_'.$membership;
    $datafieldname = $fieldname.'_data';
    $fieldval = $vars->$fieldname;
    if ($fieldval['und'][0]['value'] == 1) {
      $datafieldval = $vars->$datafieldname;
      $data = $datafieldval['und'][0]['value'];
      $output .= _BDW_get_membership_icon($basepath . $details[0], 
        $details[1], $data, $details[2], $display_data);
    }
  }
  $output .= "</div>";
  echo $output;
}

function BDW_has_group_memberships($vars) {
  $memberships = _BDW_get_memberships();
  foreach ($memberships as $membership => $details)
  {
    $fieldname = 'field_'.$membership;
    $datafieldname = $fieldname.'_data';
    $fieldval = $vars->$fieldname;
    if (!empty($fieldval) && $fieldval['und'][0]['value'] == 1) {
      return TRUE;
    }
  }
  return FALSE;
}

function _BDW_get_membership_icon($image, $alttext, $data = NULL, $data_title = NULL, $display_data = TRUE)
{
  $output = "<div class='MembershipBadge'>";
  $output .= "<span title='$alttext'>";
  if (isset($data) && $display_data)
  {
    if (isset($data_title))
    {
      $data = "<h3>$data_title:</h3> $data";
    }
    $attributes = array(
      'text' => $data,
      'image' => $image,
      'title' => $alttext,
      'id' => 'memberbadge',
      'style' => 'obsidian',
      'class' => 'badgepopup',
      'width' => '100',
      'origin' => 'top-left',
      'expand' => 'bottom-right',
      'close' => 'TRUE',
    );
    module_load_include('inc', 'popup', 'includes/popup.api');
    $retval = popup($attributes);
    $output .= $retval;
  }
  else
  {
    $output .= "<img src='/sites/boondockerswelcome.com/files/$image' alt='$alttext'/>";
  }
  $output .= "</span>";
  $output .= "</div>";
  
  return $output;
}

