#run using python3!
import logging
import aiohttp
import asyncio
import json
import time
import sys
import re #regex


''' 
TODO:
- tighten up requirements for valid syntax 
   - 50 km max???
   - etc

- use ta test cases
- read spec thoruoghly 

'''


API_KEY = REMOVED
IP_ADDR = '127.0.0.1'

connections = {
    'Hill'      : [1,2],
    'Jaquez'    : [0,3],
    'Smith'     : [0,3,4],
    'Singleton' : [1,2,4],
    'Campbell'  : [2,3]
}

ports = {
    0 : 11715,
    1 : 11716,
    2 : 11717,
    3 : 11718,
    4 : 11719,
    'Hill'      : 11715,
    'Jaquez'    : 11716,
    'Smith'     : 11717,
    'Singleton' : 11718,
    'Campbell'  : 11719,
    11715 :  'Hill'     ,
    11716 : 'Jaquez'    ,
    11717 : 'Smith'     ,
    11718 : 'Singleton' ,
    11719 : 'Campbell' 
}

received_messages = []
myConns = []
serverName = ''

#def main():
async def main():
    global serverName, myConns
    try:
        serverName = sys.argv[1]
    except IndexError:
        print('Error!\nCall server.py with one argument: the name of the server')
        exit()

    if serverName not in ['Hill', 'Jaquez', 'Singleton', 'Smith', 'Campbell']: 
        print('Error!\nServer name must be Hill, Jaquez, Singleton, Smith, or Campbell')
        exit()
    else:
        myConns = [ports[x] for x in connections[serverName]] #which pots should comm with 

    # logging help: https://docs.python.org/3/howto/logging.html - use logging.{debug, info or warning}
    logfileName = 'Server-' + serverName + '.log'
    open(logfileName, 'w').close() #to clear old log if exists
    logging.basicConfig(filename = logfileName,   level=logging.DEBUG,
                        format='%(asctime)s %(message)s', datefmt='%m/%d/%Y %I:%M:%S %p')
    logging.info('Server {} started'.format(serverName))

    #    await(simple_case(url))
    #await asyncio.run(simple_case(url))
    
    try:
        #trying from ta hint code @ https://github.com/CS131-TA-team/UCLA_CS131_CodeHelp/blob/master/Python/echo_server.py becuase can't talk to other servers using higher level api for some reason
#        loop = asyncio.get_event_loop()
#        coro = asyncio.start_server(handle_connection, IP_ADDR, ports[serverName], loop = loop)
#        server = loop.run_until_complete(coro)
#        loop.run_forever()
        server = await asyncio.start_server(handle_connection, host=IP_ADDR, port=ports[serverName])
        await server.serve_forever()
    except KeyboardInterrupt:
        logging.info('Application ending due to user keyboard interrupt')
    except SystemExit:
        logging.info('Application exiting due to fatal error')
#    except:
#        print("test")
        #    finally:
#        server.close()
#        loop.run_until_complete(server.wait_closed())
#        loop.close()
    
async def sendToMyConns(data):
    global myConns
    for port in myConns:
        try:
            reader, writer = await asyncio.open_connection(IP_ADDR, port) # add loop?
            writer.write(data.encode())
            await writer.drain()
            writer.close()
            logging.info('Sent "{}" to {}'.format(data[:-1], ports[port])) #[:-1] removes last element (\n) 
        except ConnectionRefusedError:
            logging.error('Connection refused error to server {} with port {}'.format(ports[port], port))
        except:
            logging.error('Unable to connect to server {} using port {}'.format(ports[port], port))
        
async def flood(data): #data in decoded format
    global received_messages
    if data not in received_messages:
        received_messages.append(data)
        await sendToMyConns(data)

async def pingGoogleAPI(url):
    async with aiohttp.ClientSession(  connector=aiohttp.TCPConnector(ssl=False,),) as session:
        async with session.get(url) as resp:
            response = await resp.json()
            #print ("JSON VERSION :\n\n" +  str(response))
#            response = await resp.text()
            #print("TEXT VERSION: \n\n" + temp)
            return response
    
async def handle_connection(reader, writer):
    inBinary = await reader.readline()
    inp = inBinary.decode()
    logging.info('Received: "{}"'.format(re.sub('\n$', '', inp))) 
    timeRecv = time.time()
    data = inp.strip().split()
    errorMsg = '? {}'.format(inp)
    toRespond = errorMsg 
    send = 1
    if len(data) != 4:
        if len(data) == 6 and data[0] == 'AT': #comms from another server
            send = 0 # dont respond to them
            await flood(inp) # this will still respond to them though :/
        elif len(data) == 1 and data[0] == 'sudoQUIT': #if have to force all to shutdown and dont have access to pid
            await flood(inp)
            print('quit command received')
            logging.info('quit command received')
            sys.exit()
        else:
            toRespond = errorMsg
            logging.error('input not 4 elements')
    elif data[0] == 'IAMAT':
        isvalid = 0 #checking for valid time and coordinates
        try:
            timeDiff = timeRecv - float(data[3])
            isvalid = 1
            if timeDiff > 0:
                timeDiff = '+' + str(timeDiff)
            else:
                timeDiff = str(timeDiff)
        except ValueError:
            logging.error('Invalid time')
        if (isvalid):
            try:
                isvalid = 0
                coords = data[2]
                if (coords[0] == '+' or coords[0] == '-'):
                    temp = coords[1:]
                    tempMin = temp.split('-')
                    if len(tempMin) == 2:
                        float(tempMin[0])
                        float(tempMin[1])
                        isvalid = 1
                    elif len(temp.split('+')) == 2:
                        tempPlus = temp.split('+')
                        float(tempPlus[0])
                        float(tempPlus[1])
                        isvalid = 1
            except ValueError:
                isvalid = 0

        if isvalid:
            outStr = ' '.join(data[1:]) # get rid of IAMAT part
            toRespond = 'AT {} {} {}'.format(sys.argv[1], timeDiff, outStr) # AT SERV_NAME TIME_DIFF copy data-iamat
            await flood(toRespond) #flood other servers
        else:
            logging.error('Invalid coordinates')
        
    elif data[0] == 'WHATSAT':
        name = data[1]
        tempLoc = 'null'
        for msg in received_messages:
            msgData = msg.split(' ')
            if name == msgData[3]:
                tempLoc =  msgData[4]
                toRespond = msg + '\n'

        isvalid = 0
        if(tempLoc == 'null'):
            logging.error('Could not find any client with that name')
        else:
            isvalid = 1
            #sys.exit()
            #CHANGE????
            
        #        isvalid = 0
        if isvalid: #checking for valid lats/longs and creating loc format for google api
            tempLoc = tempLoc[1:] # remove front +/-
            tempPlus = tempLoc.split('+')
            isvalid = 0
            if len(tempPlus) == 2: 
                latitude = tempPlus[0]
                longitude = '+' + tempPlus[1]
                isvalid = 1
            else:
                tempMinus = tempLoc.split('-')
                if len(tempMinus) == 2:
                    latitude = tempMinus[0]
                    longitude = '-' + tempMinus[1]
                    isvalid = 1

            if isvalid:
                loc = '{0},{1}'.format(latitude, longitude)
                url = 'https://maps.googleapis.com/maps/api/place/nearbysearch/json?key={0}&location={1}&radius={2}'.format(API_KEY, loc, data[2])
                googleResponse = await pingGoogleAPI(url)
                #print(googleResponse)
                googleResponse['results'] = googleResponse['results'][:int(data[3])]
                #regex in python help: https://stackoverflow.com/questions/7068751/regex-to-find-two-or-more-consecutive-chars/7068787
                temp = json.dumps(googleResponse)
                temp1 = re.sub('\n{2,}', '\n', str(temp)) #substitutue any 2+ consec newlines w one
                #temp2 = re.sub('\'', '\"', temp1)
                toRespond += re.sub('\n+$', '\n\n', temp1) #remove any trailing newlines and add two
                
            else:
                logging.error('Invalid WHATSAT query')
        else:
            toRespond = errorMsg
#            logging.error('Could not find any client with that name')
    else:
        logging.info('Unknown command')
        toRespond = errorMsg

    if(send):
        logging.info('Responding: \"{}\"'.format(re.sub('\n$', '', toRespond)))
        writer.write(toRespond.encode())
        await writer.drain()
    writer.close()

    
if __name__ == '__main__':
#    try:
    asyncio.run(main())
#    except KeyboardInterrupt:    works...
#        print("caught interrupt")
        #    main()
