import { useState, useEffect } from 'react'
import './App.css'
import car from './car.png'
import light from './light.svg'
import lightoff from './lightoff.svg'

function App() {
  const [color, setColor] = useState('green');
  const [lightstatus, setLightStatus] = useState(false);
  const [data, setData] = useState(null);
  const style = {
    borderColor: `${color} transparent transparent transparent`
  };
  
  useEffect(() => {
      const fetchData = async () => {
          try {
              const response = await fetch('http://127.0.0.1:8080/data');
              if(response.ok){
                const ret = await response.json()
                console.log(ret)

                setData(ret.distance);
                if (ret.distance < 10) {
                  setColor('red');
                }
                else if (ret.distance < 25) {
                  setColor('yellow');
                } 
                else if(ret.distance > 30) {
                  setColor('green');
                }
                setLightStatus(ret.light);
              }
             
              
          } catch (error) {
              console.error('Failed to fetch data:', error);
          }
      };

      // Call fetchData once when the component mounts
      fetchData();

      // Set up the polling interval
      const interval = setInterval(fetchData, 500); // Adjust the polling interval to your needs

      // Cleanup function to clear the interval when the component unmounts
      return () => clearInterval(interval);
  }, []);

  return (
    <div className='container'>
      <div className='info'>
        <div className="signal">
          <div className="half-cirle" style={style}></div>
          <div className="half-cirle" style={style}></div>
          <div className="half-cirle" style={style}></div>
        </div>
        
       
        <p>{data ? data : "NaN"} cm</p>
        <div> <img className="car" src={car} alt="" /> </div>
      </div>
 

      <div className='lightcontainer'>
        {
          lightstatus ? 
          <img src={light} className="light" alt="" /> :
          <img src={lightoff} className="light" alt="" />
          
        }
        {
        lightstatus?
        <label style={{color:"green"}}>Light On</label>
        :<label style={{color:"red"}}>Light off</label>
      }
      </div>
      
    </div>

  )
}

export default App
