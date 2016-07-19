//  1. Enter sheet name where data is to be written below
        var SHEET_NAME = "Sheet1";

//  2. Run > setup
//
//  3. Publish > Deploy as web app
//    - enter Project Version name and click 'Save New Version'
//    - set security level and enable service (most likely execute as 'me' and access 'anyone, even anonymously)
//
//  4. Copy the 'Current web app URL' and post this in your form/script action
//
//  5. Insert column names on your destination sheet matching the parameter names of the data you are passing in (exactly matching case)

var SCRIPT_PROP = PropertiesService.getScriptProperties(); // new property service

// If you don't want to expose either GET or POST methods you can comment out the appropriate function
function doGet(e){
  return handleResponse(e);
}

function doPost(e){
  return handleResponse(e);
}

function handleResponse(e) {
  // shortly after my original solution Google announced the LockService[1]
  // this prevents concurrent access overwritting data
  // [1] http://googleappsdeveloper.blogspot.co.uk/2011/10/concurrency-and-google-apps-script.html
  // we want a public lock, one that locks for all invocations
  var lock = LockService.getPublicLock();
  lock.waitLock(30000);  // wait 30 seconds before conceding defeat.
  var val = e.parameter.value;
  try {
    // next set where we write the data - you could write to multiple/alternate destinations

//  var SPREADSHEET_ID = "1nSMyW1R-oKIZBjmIz5L7AuduGobxv0rUXFUutRqljAM"; //Rudi's XLS sheet
    var SPREADSHEET_ID = "1EjDXjxsU1T6WPc_2eDv9oyeZjQBobPMp4C9s1Qw9QYs"; //TPE-hackerspace XLS sheet
    var doc = SpreadsheetApp.openById( SPREADSHEET_ID );
    var sheet = SpreadsheetApp.openById( SPREADSHEET_ID ).getSheetByName("ESP-8266");

    var now = Utilities.formatDate(new Date(), "EST", "yyyy-MM-dd'T'hh:mm a'Z'").slice(11,19);
    sheet.getRange('B2').setValue(now);               //it adds a time stamp
    sheet.getRange('A2').setValue(val);  //here we will input the ID of the user

    // return json success results
    return ContentService
          //.createTextOutput(JSON.stringify({"result":"success", "row": nextRow}))
          .createTextOutput(sheet.getRange('A2').getValue())
          .setMimeType(ContentService.MimeType.JSON);
  } catch(e){
    // if error return this
    return ContentService
          .createTextOutput(JSON.stringify({"result":"error", "error": e}))
          .setMimeType(ContentService.MimeType.JSON);
  } finally { //release lock
    lock.releaseLock();
  }
}

function setup() {
    var doc = SpreadsheetApp.getActiveSpreadsheet();
    SCRIPT_PROP.setProperty("key", doc.getId());
}
