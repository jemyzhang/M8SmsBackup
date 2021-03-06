-- TABLE_SMS v2
create table if not exists 'sms_v2' 
('Name' text not null, 'PN' text not null, 'PhoneNumber' text not null, 'Content' text not null,
'TimeStamps' datetime not null, 'SendReceive' numeric not null, 'Year' text, 'Month' text, 'Day' text);

-- TABLE_CONTACT v1
create table if not exists 'contacts_v1' 
('PhoneNumber' text primary key, 'Name' text not null, 'Label' numeric not null);

-- TABLE_CONTACT v2, add LastName/FirstName Groupid
create table if not exists 'contacts_v2' 
('PhoneNumber' text primary key, 'FullName' text not null, 
'LastName' text not null, 'FirstName' text not null, 'Label' numeric not null, 'GroupID' numeric not null);

-- TABLE_GROUP v1
create table if not exists 'groups_v1' 
('id ' numeric primary key, 'name' text not null);

-- TABLE_FILTER v1
create table if not exists 'filter_v1' 
('PhoneNumber' text primary key, 'Enabled' bool not null default 1)
