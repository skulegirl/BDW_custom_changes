<?php

/**
 * Custom function to see if a specific file has been downloaded yet by the
 * current user.
 * This replaces all the functions below, since they use ubercart, and this
 * just uses a plain file download, supported by download_count module.
 */
function BDW_custom_changes_FileHasBeenDownloaded ($filename) {
  global $user;

  $result = db_query("SELECT COUNT(dc.dcid) AS count FROM {download_count} dc JOIN {files} f ON dc.fid = f.fid WHERE f.filename = '%s' AND dc.uid = %d", $filename, $user->uid);

  while ($download = db_fetch_object($result)) {
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

  $file_downloads = db_query("SELECT * FROM {uc_file_users} AS ufu INNER JOIN {uc_files} AS uf ON ufu.fid = uf.fid WHERE ufu.uid = %d ORDER BY uf.filename ASC", $user->uid);
  while ($file_download = db_fetch_object($file_downloads)) {
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
  $file_downloads = db_query("SELECT * FROM {uc_file_users} AS ufu INNER JOIN {uc_files} AS uf ON ufu.fid = uf.fid INNER JOIN uc_file_products as p ON p.pfid = ufu.pfid WHERE ufu.uid = %d ORDER BY uf.filename ASC", $user->uid);
  while ($file_download = db_fetch_object($file_downloads)) {
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
  return db_result(db_query("SELECT COUNT(*) FROM {flag_content} INNER JOIN {users} ON flag_content.content_id=users.uid WHERE flag_content.fid = 2 AND flag_content.uid = 0"));
}

?>
