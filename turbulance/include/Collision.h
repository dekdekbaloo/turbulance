#ifndef COLLISION_H
#define COLLISION_H
#define RADIUS 0.05f
#include "algebra3.h"
#include "Particle.h"
class Collision
{
    private:
        float penetration = 0.0f;
        vec3 n=vec3 (0,0,0);
    public:
        Collision();
        virtual ~Collision();
        bool collide(Particle a,Particle b){
            vec3 dv=a.r-b.r;
            float d2=dv.length2();
            if(d2>RADIUS*RADIUS)return false;
            float r=RADIUS+RADIUS;
            float d=dv.length();
            if(d!=0){
                penetration=r-d;
                n=(b.r-a.r)/d;
                return true;
            }
            else {
                penetration=RADIUS;
                n=vec3 (1,0,0);
                return true;
            }
        }
        void resolve(Particle& a,Particle& b){
            vec3 rv=b.v-a.v;
            float velAlongNormal=rv*n;
            if(velAlongNormal>0)return;
            cout<<velAlongNormal<<endl;
            float e=0.8f;//bounce efficiency
            //impulse
            float j=-(1-e)*velAlongNormal;
            j/=2;
            //apply
            vec3 i=j*n;
            a.v-=i;
            b.v+=i;
        }
        void correction(Particle& a, Particle& b){
            float percent=0.8f;
            float slop=0.05f;
            //cout<<penetration<<endl;
            vec3 correct=(penetration-slop)>0?(penetration-slop/2)*percent*n:0;
            cout<<n<<endl;
            a.r-=correct;
            b.r+=correct;
        }


};

#endif // COLLISION_H
