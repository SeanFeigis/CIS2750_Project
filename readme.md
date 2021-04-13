# Assignment 4

Name: Sean Shaya Feigis 

ID: 1096849

Due Date: April 15, 2021

## Usage

```bash
 npm install
```

```bash
 cd parser
 make parser
```

```bash
npm run dev <port ID>
```

## Website Layout

The first part of the website is the Assignment 3 functionality and can be ignored except for Rename and Add Route. For rename, select a file from the drop down GPX View panel drop down, select a route from the Rename Route Drop Down, and then type in your new route and click the Rename Button. 

For Add Route, Select a file from the drop down, add as many waypoints as you'd like and enter the information and press the Add Route Button.

# Assignment 4 Layout

The assignment 4 functionality begins at the bottom of the page under the 'Assignment 4' Header

## Login

The first thing you'll see is the login. Type in the information and press the 'login' button to login. If the login is successful, an alert will pop up notifying you that you logged in successfuly. If the login fails for any reason, a different alert will pop up to notify you of the failure. 
Please note that after a successful login, the form will only change the login information on correct logins. So if you login correctly and then attempt to login with incorrect information, the new incorrect information will not be stored and you will stay logged in.

All other A4 functionality is disabled until a successful login occurs.

## Store Files, Clear All Data, Display Status

The store file button is the first button in that section. It will only be enabled if there are >=1 file on the server. If the database is empty prior to the click, it should enable clear data and the query selection.

Clear all Data is the second button, and should only be enabled when there are non empty tables on the server. Pressing the button empties the table, as well as disabling the queries and button itself.

DisplayDB Status is the next button and will display the current DB status. It will only be enabled.

## Queries

The Queries section has a drop down which contains the 5 queries from the assignment, in the order of the assignment description. On change of the drop down, the other drop down selections will hide/unhide as needed for that query. Pressing the query will run the query and display the results in the Query Table at the bottom.



